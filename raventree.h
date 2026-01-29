#ifndef RAVENTREE_H
#define RAVENTREE_H

#include "gitmanager.h"
#include "raventreemodel.h"

#include <QTreeView>
#include <QWidget>

class RavenLHSView;

class RavenTree : public QTreeView
{
    Q_OBJECT
public:
    explicit RavenTree(GitManager *gitManager, QWidget *parent = nullptr);

    RavenTreeModel *model() const { return m_model; }
    int getMaxStatusFilesCount() { return MAX_STATUS_FILES_COUNT; }
signals:
    void renderDiffItem(GitManager::GitDiffItem item);

public slots:
    void buildTree(QString repoPath, GitManager::status_data payload);
    void onFileOpened(const QModelIndex &index);

private:
    RavenTreeModel *m_model;
    RavenLHSView *m_lhsView;
    GitManager *m_gitManager;

    bool maxStatusFilesCountReached = false;
    int MAX_STATUS_FILES_COUNT = 500;

    void onStageItem(RavenTreeItem *treeItem);
    void onUnstageItem(RavenTreeItem *treeItem);
    void mouseReleaseEvent(QMouseEvent *event) override;

    struct RavenTreeBuildHelper {
        QString repoPath;
        RavenTreeItem *currentNode;
        QString path;
        QStringList split;

        GitManager::GitStatusItem status;
    };
    void _buildTree(RavenTreeBuildHelper helper);
};

#endif // RAVENTREE_H
