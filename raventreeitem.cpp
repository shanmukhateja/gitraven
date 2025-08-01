#include "raventreeitem.h"
#include <qdebug.h>

RavenTreeItem::RavenTreeItem() {}

bool RavenTreeItem::checkIfFileDeleted(git_status_t status)
{
    if (status == GIT_STATUS_WT_DELETED || status == GIT_STATUS_INDEX_DELETED) return true;

    // file is new to index but deleted in working directory
    if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_DELETED)) return true;

    // file exists in index but deleted in working directory
    if (status == (GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_DELETED)) return true;

    // file is new to working directory but deleted in index
    if (status == (GIT_STATUS_INDEX_DELETED|GIT_STATUS_WT_NEW)) return true;

    return false;
}

RavenTreeItem::RavenTreeCategory RavenTreeItem::getTreeCategoryByStatus(git_status_t status)
{
    bool uncommitted =
        status == GIT_STATUS_WT_NEW            ||
        status == GIT_STATUS_WT_MODIFIED       ||
        status == GIT_STATUS_WT_DELETED        ||
        status == GIT_STATUS_WT_TYPECHANGE     ||
        status == GIT_STATUS_WT_RENAMED        ||
        status == GIT_STATUS_WT_UNREADABLE
        ;

    // Could be a new file that was staged and it contains some uncommitted changes.
    if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_MODIFIED))
    {
        // qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_MODIFIED)";
        return RavenTreeCategory::BOTH;
    }

    // Could be a new file that was staged but it is now deleted in working directory
    if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_DELETED))
    {
        // qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_DELETED)";
        return RavenTreeCategory::BOTH;
    }

    // Could be a new file that was staged but it is renamed in working directory
    if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_RENAMED))
    {
        // qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_RENAMED)";
        return RavenTreeCategory::BOTH;
    }

    // Could be a file already staged and modified but it contains some uncommitted changes
    if (status == (GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_MODIFIED))
    {
        // qDebug() << "(GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_MODIFIED)";
        return RavenTreeCategory::BOTH;
    }

    // Could be a file already staged and modified but it is now deleted in working directory
    if (status == (GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_DELETED))
    {
        // qDebug() << "(GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_DELETED)";
        return RavenTreeCategory::BOTH;
    }

    return uncommitted ? RavenTreeCategory::UNCOMMITTED : RavenTreeCategory::STAGING;
}
