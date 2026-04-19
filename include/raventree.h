#ifndef RAVENTREE_H
#define RAVENTREE_H

#include "gitmanager.h"
#include "raventreemodel.h"

#include <QTreeView>
#include <QWidget>

class RavenLHSView;

class RavenTree : public QTreeView {
    Q_OBJECT
  public:
    explicit RavenTree(QWidget *parent = nullptr);

    [[nodiscard]] RavenTreeModel *model() const;
    int getMaxStatusFilesCount() { return MAX_STATUS_FILES_COUNT; }
  signals:
    void renderDiffItem(GitManager::GitDiffItem item);

  public slots:
    void buildTree(QString repoPath, GitManager::status_data payload);
    void onFileOpened(const QModelIndex &index);

  private:
    RavenTreeModel *m_model;
    RavenLHSView *m_lhsView;
    GitManagerBase *m_gitManager;

    bool maxStatusFilesCountReached = false;
    int MAX_STATUS_FILES_COUNT = 500;

    void onStageItem(RavenTreeItem *treeItem);
    void onUnstageItem(RavenTreeItem *treeItem);
    void onDeleteRequested(RavenTreeItem *treeItem);
    void onOpenNodeInFMRequested(RavenTreeItem *treeItem);

    void mouseReleaseEvent(QMouseEvent *event) override;

    struct RavenTreeBuildHelper {
        QString repoPath;
        RavenTreeItem *currentNode;
        QString path;

        GitManager::GitStatusItem status;
    };
    void _buildTree(RavenTreeBuildHelper &helper);

    void initCustomActions();
    void buildContextMenuForTreeItem(RavenTreeItem *treeItem);
    void OnContextMenuRequested(const QPoint &pos);

    QMenu *m_contextMenu;
    QAction *m_deleteAction;
    QAction *m_stageAction;
    QAction *m_unstageAction;
    QAction *m_openNodeInFMAction;
    QList<QAction *> m_contextMenuActionsList;
};

#endif // RAVENTREE_H
