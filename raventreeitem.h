#ifndef RAVENTREEITEM_H
#define RAVENTREEITEM_H

#include <QObject>
#include <git2/status.h>


class RavenTreeItem : public QObject
{
    Q_OBJECT
public:

    enum RavenTreeCategory {
        STAGING,
        UNCOMMITTED,
        BOTH
    };

    RavenTreeItem();
    QString name;
    QString fullPath;
    QString absolutePath;
    bool binary;
    QList<RavenTreeItem*> children;
    git_status_t flag;
    // Used by UI for determining which category tree we belong to.
    RavenTreeCategory initiator;
    // Used by UI to disallow opening diff when root nodes are clicked.
    bool heading;
    // Used by UI to check if item is deleted.
    bool deleted;
    // Used by UI to show status as file is modified
    bool modified() {
        return flag == GIT_STATUS_WT_MODIFIED || flag == GIT_STATUS_INDEX_MODIFIED;
    }

    static RavenTreeCategory getTreeCategoryByStatus(git_status_t status);
    static bool checkIfFileDeleted(git_status_t status);
    std::optional<QStringConverter::Encoding> encodingOpt;
};

#endif // RAVENTREEITEM_H
