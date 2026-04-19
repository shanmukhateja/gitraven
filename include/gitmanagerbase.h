//
// Created by suryateja on 19/04/26.
//

#ifndef GITRAVEN_GITMANAGERBASE_H
#define GITRAVEN_GITMANAGERBASE_H

#include <git2.h>

#include "raventreeitem.h"

class GitManagerBase : public QObject {
    Q_OBJECT
  public:
    explicit GitManagerBase(QObject *parent = nullptr, QString repoPath = nullptr)
        : QObject(parent), m_repoPath(repoPath) {}
    virtual ~GitManagerBase() {}

    virtual int init() { return 0; }

    enum GitHEADStatusType { GIT_HEAD_TYPE_COMMIT = 1, GIT_HEAD_TYPE_BRANCH = 2, GIT_HEAD_TYPE_TAG = 3 };
    struct GitBranchSelectorItem {
        GitHEADStatusType type;
        QString name;
        bool isRemote;
    };

    struct GitHEADStatus {
        QString name;
        GitHEADStatusType type;
    };
    struct GitStatusItem {
        QString path;
        git_status_t flag;
        RavenTreeItem::RavenTreeCategory category;
        bool deleted;
    };

    // Struct to send/receive data between `status()` and `status_cb()`
    typedef struct status_data {
        QList<GitStatusItem> statusItems;
    } status_data;

    struct GitDiffItem {
        git_oid oldOid;
        QString oldFilePath;
        QString oldFileContent;

        git_oid newOid;
        QString newFilePath;
        QString newFileContent;

        bool binary;

        RavenTreeItem::RavenTreeCategory category;
    };

    enum GitStageResponseCode {
        INDEX_NOT_FOUND,
        NOT_IN_INDEX,
        IS_CONFLICT,
        FAILED_TO_INDEX,
        FAILED_TO_UNSTAGE,
        FAILED_WRITE_INDEX_TO_DISK,
        UNKNOWN,
        DONE
    };

    // FIXME: This feels wrong.
    struct FileItem {
        QString content;
        bool binary;
    };

    typedef struct diff_data {
        git_oid old_oid;
        git_oid new_oid;

        QString oldPath;
        QString oldAbsPath;
        QString newPath;
        QString newAbsPath;

        git_delta_t status;

        QString reqFilePath;
        QString repoPath;
    } diff_data;

    // FIXME: This looks unsafe.
    git_repository *getRepo() { return m_repo; }
    QString getRepoPath() { return m_repoPath; }
    virtual void statusAsync() = 0;
    virtual GitHEADStatus findHEADStatus() = 0;
    virtual GitDiffItem diff(RavenTreeItem *item) = 0;
    virtual GitStageResponseCode stageItem(RavenTreeItem *item) = 0;
    virtual GitStageResponseCode unstageItem(RavenTreeItem *item) = 0;
    virtual int commit(QList<QString> items, QString msg, bool amend) = 0;
    virtual QList<GitBranchSelectorItem> getAllBranchesAndTags() = 0;
    /**
     * @brief checkoutToRef
     * Checkout in `libgit2` doesn't switch branch, it simply checks files out on disk.
     * Hence, we must call `git_repository_set_head` on success.
     * More info: https://stackoverflow.com/a/46758861
     * @param item The payload contains data used to checkout to ref (branch/tag)
     * @return `nullptr` on success or QString with error message.
     */
    virtual QString checkoutToRef(GitBranchSelectorItem item) = 0;
    // protected:
    git_repository *m_repo = nullptr;
    QString m_repoPath = nullptr;

    virtual std::optional<FileItem> getFileContent(git_oid oid) = 0;
    virtual std::optional<FileItem> getLocalFileContent(const QString &absPath) = 0;

    // This MUST be called in constructor only.
    // virtual int init() = 0;

    static int each_file_cb(const git_diff_delta *delta, float progress, void *payload);
    static int each_binary_file_cb(const git_diff_delta *delta, const git_diff_binary *binary, void *payload);

    virtual QString oid_to_str(git_oid oid) = 0;

    virtual QString getCheckoutErrorMessage() = 0;
    virtual std::string generateRefName(GitBranchSelectorItem *item) = 0;
  signals:
    void statusChanged(status_data payload);
};

#endif // GITRAVEN_GITMANAGERBASE_H
