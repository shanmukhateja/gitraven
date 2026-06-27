#ifndef RAVENTREEITEM_H
#define RAVENTREEITEM_H

#include <QObject>
#include <git2/status.h>

class RavenTreeItem : public QObject {
    Q_OBJECT
  public:
    enum RavenTreeCategory { STAGING, UNCOMMITTED, BOTH };
    enum TreeItemType { FOLDER, FILE };

    explicit RavenTreeItem(QObject *parent = nullptr);
    ~RavenTreeItem() override;

    QString name;
    QString relativePath;
    QString absolutePath;
    bool binary;
    QList<RavenTreeItem *> children;
    // Used by UI for determining given node is a folder (0) or file (1)
    [[nodiscard]] auto type() const -> TreeItemType { return children.isEmpty() ? FILE : FOLDER; }
    git_status_t flag;
    // Used by UI for determining which category tree we belong to.
    RavenTreeCategory initiator;
    // Used by UI to disallow opening diff when root nodes are clicked.
    bool heading;
    // Used by UI to check if item is deleted.
    bool deleted;
    // Used by UI to show status as file is modified
    [[nodiscard]] bool modified() const { return flag == GIT_STATUS_WT_MODIFIED || flag == GIT_STATUS_INDEX_MODIFIED; }

    static RavenTreeCategory getTreeCategoryByStatus(git_status_t status);
    static bool checkIfFileDeleted(git_status_t status);
    std::optional<QStringConverter::Encoding> encodingOpt;
};

#endif // RAVENTREEITEM_H
