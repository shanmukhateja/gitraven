#include "ravengitstatusthread.h"

#include <git2/status.h>
#include <QDebug>

RavenGitStatusThread::RavenGitStatusThread(QObject *parent)
    : QThread{parent},
    m_gitManager{(GitManager*)parent}
{
    // These options ensure we return the same output as `git status`
    // Note: This should still be marked as a known issue just in case.
    // FIXME: Replace `GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS` with lazy-loaded
    //        "get" of files inside directory
    m_statusOpts = {
        .version = GIT_STATUS_OPTIONS_VERSION,
        .show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR,
        .flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
                 GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                 GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
                 GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
                 GIT_STATUS_OPT_SORT_CASE_SENSITIVELY
    };
}

void RavenGitStatusThread::run()
{
    qDebug() << "RavenGitStatusThread::run() called";
    GitManager::status_data payload = {.statusItems = {}};

    auto repo = m_gitManager->getRepo();
    git_status_foreach_ext(repo, &m_statusOpts, status_cb, (void*)&payload);
    emit resultReady(payload);
}

int RavenGitStatusThread::status_cb(const char *path, unsigned int status_flags, void *payload)
{
    auto *d = (GitManager::status_data*)payload;
    auto statusItem = GitManager::GitStatusItem {path, (git_status_t)status_flags };
    statusItem.category = RavenTreeItem::getTreeCategoryByStatus(statusItem.flag);
    statusItem.deleted = RavenTreeItem::checkIfFileDeleted(statusItem.flag);

    d->statusItems.push_back(statusItem);
    return 0;
}
