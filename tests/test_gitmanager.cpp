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

bool GitManagerTests::cleanupBeforeTestCase() const { return m_gitBinaryRunner->resetGitRepo(); }

void GitManagerTests::cleanupTestCase() const { QCOMPARE(m_gitBinaryRunner->teardownGitRepo(), true); }
