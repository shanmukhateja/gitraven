#include "raventreemodel.h"

#include <QFont>
#include <QIcon>

RavenTreeModel::RavenTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
    rootNode(createNode("Root", "", "", GIT_STATUS_IGNORED, false, true, false, std::nullopt)),
    m_uncommittedRootNode(createNode("Changes", "", "", GIT_STATUS_IGNORED, false, true, false, std::nullopt)),
    m_stagingRootNode(createNode("Staging Area", "", "", GIT_STATUS_IGNORED, false, true, false, std::nullopt))
{
    // Define `initator` property here.
    // All child nodes will inherit this.
    m_stagingRootNode->initiator = RavenTreeItem::STAGING;
    m_uncommittedRootNode->initiator = RavenTreeItem::UNCOMMITTED;
}

QModelIndex RavenTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    RavenTreeItem *parentNode = parent.isValid() ? static_cast<RavenTreeItem*>(parent.internalPointer()) : rootNode;
    RavenTreeItem *childNode = parentNode->children.value(row);

    // Ensure that the node is valid before creating the index
    if (!childNode) QModelIndex();

    return createIndex(row, column, childNode);
}

QModelIndex RavenTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();

    RavenTreeItem *childNode = static_cast<RavenTreeItem*>(child.internalPointer());

    // If it's rootNode, return invalid QModelIndex
    if (childNode == rootNode || childNode == m_stagingRootNode || childNode == m_uncommittedRootNode) return QModelIndex();

    // Find the parent node by traversing the tree
    RavenTreeItem *parentNode = recursiveFindParentNode(rootNode, childNode);

    // Handle invalid case
    if (!parentNode) return QModelIndex();

    // Found parentNode, compute row and return expected output
    int row = parentNode->children.indexOf(childNode);
    return createIndex(row, 0, parentNode);
}

int RavenTreeModel::rowCount(const QModelIndex &parent) const
{
    // Find row for given QModelIndex (if valid) or fallback to `rootNode`
    auto node = parent.isValid() ? static_cast<RavenTreeItem*>(parent.internalPointer()) : rootNode;
    return node->children.count();
}

int RavenTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant RavenTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    RavenTreeItem *node = static_cast<RavenTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        return node->name;
    }

    if (role == Qt::DecorationRole)
    {
        auto iconName = (node->children.size() > 0) || node->heading ? "folder-symbolic" : "filename-title-amarok";
        auto iconProvider = QIcon::fromTheme(iconName);
        return iconProvider;
    }

    if (role == Qt::ToolTipRole)
    {
        return node->absolutePath;
    }

    if (role == Qt::FontRole)
    {
        QFont f;
        f.setStrikeOut(node->deleted);
        return f;
    }

    return QVariant();
}

QVariant RavenTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return "Changes";
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void RavenTreeModel::clear()
{
    beginResetModel();
    m_uncommittedRootNode->children.clear();
    m_stagingRootNode->children.clear();
    endResetModel();
}

RavenTreeItem* RavenTreeModel::createNode(
    const QString &name,
    const QString &fullPath,
    const QString &absPath,
    git_status_t flag,
    bool binary,
    bool heading,
    bool deleted,
    std::optional<QStringConverter::Encoding> encodingOpt
)
{
    RavenTreeItem *node = new RavenTreeItem;
    node->name = name;
    node->fullPath = fullPath;
    node->absolutePath = absPath;
    node->flag = flag;
    node->binary = binary;
    node->children.clear();  // Ensure the children list is initialized
    node->heading = heading;
    node->deleted = deleted;
    node->encodingOpt = encodingOpt;
    return node;
}

RavenTreeItem *RavenTreeModel::recursiveFindParentNode(RavenTreeItem *root, RavenTreeItem *child) const
{
    RavenTreeItem *parentNode = nullptr;
    for (RavenTreeItem* node : static_cast<const QList<RavenTreeItem*>>(root->children))
    {
        // We need to find file/folder by path AND by RavenTreeCategory.
        if (node->absolutePath == child->absolutePath && (node->initiator == child->initiator)) {
            parentNode = root;
            break;
        }

        parentNode = recursiveFindParentNode(node, child);

        if (parentNode) break;
    }

    return parentNode;
}
