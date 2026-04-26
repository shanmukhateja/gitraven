//
// Created by suryateja on 20/04/26.
//

#ifndef GITRAVEN_TEST_GITMANAGER_H
#define GITRAVEN_TEST_GITMANAGER_H

#include "gitbinaryrunner.h"
#include "gitmanagerbase.h"

class GitManagerTests : public QObject {
    Q_OBJECT
  private slots:
    // run init code here
    void initTestCase();
    void emitStatusChangedForPayload(GitManagerBase::status_data payload) const;

    // test cases go here
    void testEmptyRepoNewFileDiffHasNoDiff() const;
    void testCommitAFile() const;

    void testAmendCommitAFile() const;

    // run teardown/cleanup code here
    void cleanupTestCase() const;

  private:
    [[nodiscard]] bool cleanupBeforeTestCase() const;

    static QString getRepoPath() { return "/tmp/tempRepoDir"; }

    GitManagerBase *m_gitManager = nullptr;
    GitBinaryRunner *m_gitBinaryRunner = nullptr;
};

#endif // GITRAVEN_TEST_GITMANAGER_H
