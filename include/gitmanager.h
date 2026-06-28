#ifndef GITMANAGER_H
#define GITMANAGER_H

#include "gitmanagerbase.h"
#include "raventreeitem.h"

#include <QObject>

#include <git2.h>

class GitManager : public GitManagerBase {
    Q_OBJECT
  public:
    explicit GitManager(QObject *parent = nullptr, QString repoPath = nullptr);
    ~GitManager() override;

    // FIXME: This looks unsafe.
    git_repository *getRepo() { return m_repo; }
    QString getRepoPath() { return m_repoPath; }
    void statusAsync();
    GitHEADStatus findHEADStatus();
    GitDiffItem diff(RavenTreeItem *item);
    GitStageResponseCode stageItem(RavenTreeItem *item);
    GitStageResponseCode unstageItem(RavenTreeItem *item);
    int commit(QList<QString> items, QString msg, bool amend);
    QList<GitBranchSelectorItem> getAllBranchesAndTags();
    QString checkoutToRef(GitBranchSelectorItem item);

    std::optional<FileItem> getFileContent(git_oid oid) override;
    std::optional<FileItem> getLocalFileContent(const QString &absPath);

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

    // This MUST be called in constructor only.
    int init();

    static int each_file_cb(const git_diff_delta *delta, float progress, void *payload);
    static int each_binary_file_cb(const git_diff_delta *delta, const git_diff_binary *binary, void *payload);

    QString oid_to_str(git_oid oid);

    QString getCheckoutErrorMessage();
    std::string generateRefName(GitManager::GitBranchSelectorItem *item);
};

#endif // GITMANAGER_H
