//
// Created by suryateja on 19/04/26.
//

#include "mockgitmanager.h"
#include <QDebug>

MockGitManager::MockGitManager(QObject *parent) : GitManagerBase(parent) {}

MockGitManager::~MockGitManager() {}

int MockGitManager::init() { return 1; }

QString MockGitManager::checkoutToRef(GitBranchSelectorItem item) { return "OK"; }

int MockGitManager::commit(QList<QString> items, QString msg, bool amend) { return 1; }

GitManagerBase::GitDiffItem MockGitManager::diff(RavenTreeItem *item) { return {}; }

GitManagerBase::GitHEADStatus MockGitManager::findHEADStatus() { return {}; }

std::string MockGitManager::generateRefName(GitBranchSelectorItem *item) { return ""; }

QList<GitManagerBase::GitBranchSelectorItem> MockGitManager::getAllBranchesAndTags() { return {}; }

QString MockGitManager::getCheckoutErrorMessage() { return ""; }

std::optional<GitManagerBase::FileItem> MockGitManager::getFileContent(git_oid oid) { return std::nullopt; }

std::optional<GitManagerBase::FileItem> MockGitManager::getLocalFileContent(const QString &absPath) {
    return std::nullopt;
}

QString MockGitManager::oid_to_str(git_oid oid) { return ""; }

GitManagerBase::GitStageResponseCode MockGitManager::stageItem(RavenTreeItem *item) { return {}; }

GitManagerBase::GitStageResponseCode MockGitManager::unstageItem(RavenTreeItem *item) { return {}; }

void MockGitManager::statusAsync() {}
