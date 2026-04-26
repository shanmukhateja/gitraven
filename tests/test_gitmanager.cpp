//
// Created by suryateja on 20/04/26.
//

#include "test_gitmanager.h"

#include <QSignalSpy>
#include <qtestcase.h>

#include "gitbinaryrunner.h"
#include "gitmanager.h"

void GitManagerTests::initTestCase() {
    m_gitBinaryRunner = new GitBinaryRunner(getRepoPath(), this);
    QCOMPARE(m_gitBinaryRunner->initGitRepo(), true);
    m_gitManager = new GitManager(qApp, getRepoPath());
}

void GitManagerTests::emitStatusChangedForPayload(GitManagerBase::status_data payload) const {
    const QSignalSpy spy{m_gitManager, &GitManagerBase::statusChanged};
    emit m_gitManager->statusChanged(std::move(payload));

    QCOMPARE(spy.count(), 1);
}

/// add test cases below

void GitManagerTests::testEmptyRepoNewFileDiffHasNoDiff() const {
    QCOMPARE(cleanupBeforeTestCase(), true);

    const auto treeItem = new RavenTreeItem();
    treeItem->relativePath = "a.txt";
    treeItem->name = "a.txt";
    treeItem->absolutePath = getRepoPath() + "/" + treeItem->relativePath;
    treeItem->binary = false;

    const auto createFileResult = GitBinaryRunner::createFilesForPath(treeItem->absolutePath, "bbb");
    QCOMPARE(createFileResult, true);

    const auto result = m_gitManager->diff(treeItem);
    QCOMPARE(result.oldFileContent, "");
    QCOMPARE(result.newFileContent, "bbb");
}

void GitManagerTests::testCommitAFile() const {
    QCOMPARE(cleanupBeforeTestCase(), true);

    const QString fileName = "a.txt";
    const QString absolutePath = getRepoPath() + "/" + fileName;
    const QString commitMsg = QString("feat: add file '%1'").arg(fileName);
    const QString fileContent = "bbb";

    const auto treeItem = new RavenTreeItem();
    treeItem->relativePath = fileName;
    treeItem->name = fileName;
    treeItem->absolutePath = absolutePath;
    treeItem->binary = false;

    const auto createFileResult = GitBinaryRunner::createFilesForPath(treeItem->absolutePath, fileContent);
    QCOMPARE(createFileResult, true);

    const auto result = m_gitManager->commit({fileName}, commitMsg, false);
    QCOMPARE(result, 0);

    // Use git binary to fetch just the commit log
    const auto out = m_gitBinaryRunner->runGitCommandForOutput({"log", "--format=%B"});
    QCOMPARE(out.trimmed(), commitMsg);
}

void GitManagerTests::testAmendCommitAFile() const {
    QCOMPARE(cleanupBeforeTestCase(), true);

    const QString fileName = "a.txt";
    const QString absolutePath = getRepoPath() + "/" + fileName;
    const QString commitMsg = QString("feat: add file '%1'").arg(fileName);
    const QString commitMsgAmend = commitMsg + "_amend";
    const QString fileContent = "bbb";
    const QStringList gitLogCommand = {"log", "--format=%B"};

    const auto treeItem = new RavenTreeItem();
    treeItem->relativePath = fileName;
    treeItem->name = fileName;
    treeItem->absolutePath = absolutePath;
    treeItem->binary = false;

    const auto createFileResult = GitBinaryRunner::createFilesForPath(treeItem->absolutePath, fileContent);
    QCOMPARE(createFileResult, true);

    auto result = m_gitManager->commit({fileName}, commitMsg, false);
    QCOMPARE(result, 0);

    auto out = m_gitBinaryRunner->runGitCommandForOutput(gitLogCommand);
    QCOMPARE(out.trimmed(), commitMsg);

    result = m_gitManager->commit({fileName}, commitMsgAmend, true);
    QCOMPARE(result, 0);

    out = m_gitBinaryRunner->runGitCommandForOutput(gitLogCommand);
    QCOMPARE(out.trimmed(), commitMsgAmend);
}

/// test cases end

/// cleanup code stays at the bottom

bool GitManagerTests::cleanupBeforeTestCase() const { return m_gitBinaryRunner->resetGitRepo(); }

void GitManagerTests::cleanupTestCase() const { QCOMPARE(m_gitBinaryRunner->teardownGitRepo(), true); }
