//
// Created by suryateja on 19/04/26.
//

#ifndef GITRAVEN_MOCKGITMANAGER_H
#define GITRAVEN_MOCKGITMANAGER_H
#include "gitmanagerbase.h"

class MockGitManager : public GitManagerBase {
    Q_OBJECT
  public:
    explicit MockGitManager(QObject *parent = nullptr);
    ~MockGitManager() override;

    int init();
    QString checkoutToRef(GitBranchSelectorItem item) override;
    int commit(QList<QString> items, QString msg, bool amend) override;
    GitDiffItem diff(RavenTreeItem *item) override;
    GitHEADStatus findHEADStatus() override;
    std::string generateRefName(GitBranchSelectorItem *item) override;
    QList<GitBranchSelectorItem> getAllBranchesAndTags() override;
    QString getCheckoutErrorMessage() override;
    std::optional<FileItem> getFileContent(git_oid oid) override;
    std::optional<FileItem> getLocalFileContent(const QString &absPath) override;
    QString oid_to_str(git_oid oid) override;
    GitStageResponseCode stageItem(RavenTreeItem *item) override;
    GitStageResponseCode unstageItem(RavenTreeItem *item) override;
    void statusAsync() override;
};

#endif // GITRAVEN_MOCKGITMANAGER_H
