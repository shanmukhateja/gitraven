#ifndef RAVENTREEMODEL_H
#define RAVENTREEMODEL_H

#include "raventreeitem.h"
#include <QAbstractItemModel>
#include <QObject>

#include <git2/status.h>

class RavenTreeModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit RavenTreeModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void clear();

    // Helper functions
    RavenTreeItem* getRootNode() {return rootNode;};
    RavenTreeItem* getStagingNode() {return m_stagingRootNode;};
    RavenTreeItem* getUncommittedNode() {return m_uncommittedRootNode;};
    static RavenTreeItem* createNode(
        const QString &name,
        const QString &fullPath,
        const QString &absPath,
        git_status_t flag,
        bool binary,
        bool heading,
        bool deleted,
        std::optional<QStringConverter::Encoding> encodingOpt
    );
    RavenTreeItem *recursiveFindParentNode(RavenTreeItem *root, RavenTreeItem *child) const;
    bool isStagingEmpty() { return m_stagingRootNode->children.isEmpty(); }

private:
    RavenTreeItem *rootNode;
    RavenTreeItem *m_uncommittedRootNode;
    RavenTreeItem *m_stagingRootNode;

};

#endif // RAVENTREEMODEL_H
