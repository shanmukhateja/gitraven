#ifndef GITMANAGER_H
#define GITMANAGER_H

#include "raventreeitem.h"

#include <QObject>

#include <git2.h>

class GitManager: public QObject
{
    Q_OBJECT
public:
    enum GitHEADStatusType {
        GIT_HEAD_TYPE_COMMIT = 1,
        GIT_HEAD_TYPE_BRANCH = 2
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
        QString repoPath;
        GitManager::GitHEADStatus headStatus;
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

    explicit GitManager(QString dir, QObject *parent = nullptr);
    ~GitManager();

    int init();
    status_data status(bool updateUI = true);
    GitDiffItem diff(RavenTreeItem *item);
    GitStageResponseCode stageItem(RavenTreeItem *item);
    GitStageResponseCode unstageItem(RavenTreeItem *item);
    int commit(QList<QString> items, QString msg, bool amend);

signals:
    void statusChanged(GitManager::status_data payload);

private:
    git_repository *m_repo = nullptr;
    QString m_repoPath = nullptr;

    struct RavenFile {
        QString content;
        bool binary;
    };
    std::optional<RavenFile> getFileContent(git_oid oid);
    std::optional<RavenFile> getLocalFileContent(QString absPath);

    // libgit2
    static int status_cb(const char *path,
                         unsigned int status_flags,
                         void *payload);

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

    static int each_file_cb(const git_diff_delta *delta,
                     float progress,
                     void *payload);
    static int each_binary_file_cb(const git_diff_delta *delta,
                                   const git_diff_binary *binary,
                                   void *payload);

    QString oid_to_str(git_oid oid);
    GitManager::GitHEADStatus findHEADStatus();
};


#endif // GITMANAGER_H
