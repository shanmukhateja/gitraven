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
    explicit RavenTree(QWidget *parent = nullptr);

    RavenTreeModel *model() const
    {
        return m_model;
    }

    int MAX_STATUS_FILES_COUNT = 500;
signals:
    void renderDiffItem(GitManager::GitDiffItem item);
    void stageItem(const QModelIndex *index);
    void unStageItem(const QModelIndex *index);

public slots:
    void buildTree(GitManager::status_data payload);
    void onFileOpened(const QModelIndex &index);
    void onStageItemCalled(const QModelIndex *index);
    void onUnstageItemCalled(const QModelIndex *index);

private:
    RavenTreeModel *m_model;
    RavenLHSView *m_lhsView;

    bool maxStatusFilesCountReached = false;

    void mousePressEvent(QMouseEvent *event) override;

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
