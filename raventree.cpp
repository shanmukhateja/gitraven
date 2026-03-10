#include "raventree.h"
#include "raventreemodel.h"
#include "mainwindow.h"
#include "ravenfile.h"
#include "raventreedelegate.h"
#include "ravenlhsview.h"
#include "ravenutils.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QMenu>

#include <filesystem>

namespace fs = std::filesystem;

RavenTree::RavenTree(GitManager *gitManager, QWidget *parent)
    : QTreeView{parent},
    m_gitManager(gitManager),
    m_model(new RavenTreeModel(this)),
    m_contextMenu{new QMenu(this)}
{
    m_lhsView = (RavenLHSView*) parent;

    // Update tree when Git status changes
    connect(m_gitManager, &GitManager::statusChanged, this,[this](GitManager::status_data sd){
        buildTree(m_gitManager->getRepoPath(), sd);
    }, Qt::QueuedConnection);

    // Context menu
    initCustomActions();
    // Set model
    setModel(m_model);
    // Enable mouse tracking so we can stage/unstage items
    setMouseTracking(true);
    // Use custom delegate to render custom UI elements.
    setItemDelegate(new RavenTreeDelegate());

    // click listener
    connect(this, &QAbstractItemView::activated, this, &RavenTree::onFileOpened);
    connect(this, &QAbstractItemView::clicked, this, &RavenTree::onFileOpened);
}

void RavenTree::initCustomActions()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &RavenTree::OnContextMenuRequested);
}

void RavenTree::buildContextMenuForTreeItem(RavenTreeItem *treeItem)
{
    // Clear previous values
    m_contextMenuActionsList.clear();
    m_contextMenu->clear();

    // Open file/folder in File Manager
    if (!treeItem->deleted)
    {
        #if defined(__linux__)
        m_openNodeInFMAction = new QAction(QIcon::fromTheme("open-link"), "Open in File Manager", m_contextMenu);
        connect(m_openNodeInFMAction, &QAction::triggered, this, [this, treeItem]() {
            onOpenNodeInFMRequested(treeItem);
        });
        m_contextMenuActionsList.append(m_openNodeInFMAction);
        #endif
    }

    if (treeItem->initiator == RavenTreeItem::UNCOMMITTED)
    {
        // Stage action
        m_stageAction = new QAction(QIcon::fromTheme("list-add"), "Stage", m_contextMenu);
        connect(m_stageAction, &QAction::triggered, this, [this, treeItem]{
            onStageItem(treeItem);
        });
        m_contextMenuActionsList.append(m_stageAction);

        // Delete action
        m_deleteAction = new QAction(QIcon::fromTheme("delete"), "Delete", m_contextMenu);
        connect(m_deleteAction, &QAction::triggered, this, [this, treeItem]{
            onDeleteRequested(treeItem);
        });
        m_contextMenuActionsList.append(m_deleteAction);
    }

    if (treeItem->initiator == RavenTreeItem::STAGING)
    {
        // Unstage action
        m_unstageAction = new QAction(QIcon::fromTheme("list-remove"), "Unstage", m_contextMenu);
        connect(m_unstageAction, &QAction::triggered, this, [this, treeItem]{
            onUnstageItem(treeItem);
        });
        m_contextMenuActionsList.append(m_unstageAction);
    }

    m_contextMenu->addActions(m_contextMenuActionsList);
}

void RavenTree::OnContextMenuRequested(const QPoint &pos)
{
    auto index = indexAt(pos);
    if (!index.isValid()) return;

    auto item = (RavenTreeItem*) index.internalPointer();
    if (!item->heading)
    {
        buildContextMenuForTreeItem(item);

        // HACK: menu item y-axis is too high
        QPoint *newPos = new QPoint(pos);
        newPos->setY(pos.y() + 100);

        // Show context menu
        m_contextMenu->exec(viewport()->mapFromGlobal(*newPos));
    }
}

void RavenTree::buildTree(QString repoPath, GitManager::status_data payload)
{
    qDebug() << "RavenTree::buildTree called";
    QList<GitManager::GitStatusItem> statusItems = payload.statusItems;

    RavenTreeItem *rootNode = m_model->getRootNode();
    int rowCount = m_model->rowCount();

    // Clear previous items
    if (rowCount > 0)
    {
        m_model->clear();
    }

    auto stagingRootNode = m_model->getStagingNode();
    auto uncommittedRootNode = m_model->getUncommittedNode();

    // Build tree

    // If status items count is larger than `MAX_STATUS_FILES_COUNT`,
    // inform user that we are rendering `MAX_STATUS_FILES_COUNT` items.
    if (statusItems.length() > getMaxStatusFilesCount())
    {
        this->maxStatusFilesCountReached = true;
        // Reset status count to 500
        statusItems = statusItems.first(getMaxStatusFilesCount());
        // Inform LHSView to show warning banner
        emit m_lhsView->signalMaxStatusFileCountReached(true);
    }

    for (const auto status: statusItems)
    {
        auto path = status.path;

        RavenTreeBuildHelper helper = {
            .repoPath = repoPath,
            .currentNode = rootNode,
            .path = path,
            .status = status
        };

        if (status.category == RavenTreeItem::BOTH)
        {
            helper.currentNode = m_model->getStagingNode();
            _buildTree(helper);
            helper.currentNode = m_model->getUncommittedNode();
            _buildTree(helper);
        }
        else
        {
            if (status.category == RavenTreeItem::STAGING)
            {
                helper.currentNode = m_model->getStagingNode();
            }
            if (status.category == RavenTreeItem::UNCOMMITTED)
            {
                helper.currentNode = m_model->getUncommittedNode();
            }
            _buildTree(helper);
        }
    }

    // Calculate rowCount again
    rowCount = m_model->rowCount();
    // Append child nodes to rootNode if required
    if (rowCount == 0)
    {
        rootNode->children.append(uncommittedRootNode);
        rootNode->children.append(stagingRootNode);
    }

    // emit dataChanged
    // FIXME: Figure out the QModelIndex params here.
    emit model()->dataChanged({},{}, {});

    // expand the tree if there are no staged items.
    if (stagingRootNode->children.size() == 0)
    {
        expandAll();
    }
    else
    {
        // expand staging node
        expandRecursively(m_model->index(1, 0));
    }
}

void RavenTree::_buildTree(RavenTreeBuildHelper helper)
{
    auto path = helper.path;
    auto split = path.split(std::filesystem::path::preferred_separator);
    auto currentNode = helper.currentNode;
    auto repoPath = helper.repoPath;
    auto status = helper.status;

    QString objFullPath = repoPath;

    for (auto pathPart: std::as_const(split))
    {
        // build path from pathPart
        objFullPath += std::filesystem::path::preferred_separator + pathPart;

        // Find parent node
        RavenTreeItem *expNode = new RavenTreeItem();
        expNode->absolutePath = objFullPath;
        expNode->initiator = currentNode->initiator;

        auto it = RavenTreeModel::findNodeByPathAndInitiator(currentNode, expNode);

        // Node not found in tree, create new node
        if (!it)
        {
            // generate absolute path
            fs::path repoPathStdFS(repoPath.toStdString());
            fs::path objFullPathFSPath(objFullPath.toStdString());
            fs::path fsPath(repoPathStdFS / objFullPathFSPath);
            QString objAbsPath = QString::fromStdString(fsPath);

            // TODO: migrate to RavenFile class for all this.
            RavenFile f;
            auto isBinary = f.checkFileIsBinary(objAbsPath);
            std::optional<QStringConverter::Encoding> encodingOpt = f.detectEncoding(objAbsPath);
            auto *obj = RavenTreeModel::createNode(pathPart, objFullPath, objAbsPath, status.flag, isBinary, false, status.deleted, encodingOpt);
            obj->initiator = currentNode->initiator;
            currentNode->children.append(obj);
            currentNode = obj;
        }
        else
        {
            // Parent node found.
            currentNode = it;
        }
    }
}

void RavenTree::onFileOpened(const QModelIndex &index)
{
    if (!index.isValid()) return;

    RavenTreeItem *item = static_cast<RavenTreeItem *>(index.internalPointer());

    // We do not show diff items for root nodes.
    if (item->heading) return;

    // We do not support non-UTF-8 encoded files.
    if (item->encodingOpt.has_value()) {
        auto enc = item->encodingOpt.value();
        if (enc != QStringConverter::Utf8) return;
    }

    qDebug() << "RavenTree::onFileOpened name=" << item->name;

    auto topw = topLevelWidget()->window();
    MainWindow *mainWindow = static_cast<MainWindow*>(topw);

    // Get diff item to be shared to editor.
    auto diffItem = mainWindow->getGitManager()->diff(item);
    // Update category of the item to `initiator` value.
    // This fixes issue where editor should not allow editing staged files.
    // FIXME: Should we fix it in GitManager?
    diffItem.category = item->initiator;

    // Inform editor to update view
    emit renderDiffItem(diffItem);
}

void RavenTree::mouseReleaseEvent(QMouseEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    auto index = indexAt(event->pos());
    if (index.isValid())
    {
        auto treeItem = static_cast<RavenTreeItem*>(index.internalPointer());

        auto rowRect = visualRect(index);
        auto stageOrUnstageButtonRect = rowRect;
        stageOrUnstageButtonRect.setX(rowRect.topRight().x() - 60);

        if (stageOrUnstageButtonRect.contains(mouseEvent->pos()))
        {
            auto isStagingItem = treeItem->initiator == RavenTreeItem::STAGING;
            if (isStagingItem)
            {
                onUnstageItem(treeItem);
            }
            else
            {
                onStageItem(treeItem);
            }
            event->ignore();
            return;
        }
    }
    QTreeView::mouseReleaseEvent(event);
}

void RavenTree::onStageItem(RavenTreeItem *treeItem)
{
    qDebug() << "RavenTree::onStageItem called";

    int result = m_gitManager->stageItem(treeItem);
    if (result != GitManager::GitStageResponseCode::DONE)
    {
        QStringList strings = {};
        strings.append("Failed to add file to Staging Area");
        strings.append(QString::fromStdString(std::to_string(result)));
        QMessageBox errBox(
            QMessageBox::Icon::Critical,
            "Error",
            strings.join(" "),
            QMessageBox::StandardButton::Ok,
            this
        );
        errBox.setModal(true);
        errBox.exec();
        return;
    }
    // refresh UI
    m_gitManager->statusAsync();
}

void RavenTree::onUnstageItem(RavenTreeItem* treeItem)
{
    qDebug() << "RavenTree::onUnstageItem called";

    int result = m_gitManager->unstageItem(treeItem);
    if (result != GitManager::GitStageResponseCode::DONE)
    {
        QStringList strings = {};
        strings.append("Failed to remove file to Staging Area");
        strings.append(QString::fromStdString(std::to_string(result)));
        QMessageBox errBox(
            QMessageBox::Icon::Critical,
            "Error",
            strings.join(" "),
            QMessageBox::StandardButton::Ok,
            this
            );
        errBox.setModal(true);
        errBox.exec();
        return;
    }
    // refresh UI
    m_gitManager->statusAsync();
}

void RavenTree::onDeleteRequested(RavenTreeItem *treeItem)
{
    qDebug() << "RavenTree::onDeleteRequested called";
    QMessageBox confirmDelete(QMessageBox::Question, "Delete Confirmation" , "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    confirmDelete.exec();

    if (confirmDelete.result() == QMessageBox::Yes)
    {
        // FIXME: notify user if file deletion failed
        RavenUtils::deleteFile(treeItem->absolutePath);
        m_gitManager->statusAsync();
    }
}

void RavenTree::onOpenNodeInFMRequested(RavenTreeItem *treeItem)
{
    qDebug() << "RavenTree::onOpenNodeInFMRequested called";

    RavenUtils::openNodeInFM(treeItem->absolutePath);
}

