//
// Created by suryateja on 19/04/26.
//

#include "test_raventree.h"

#include "gitmanager.h"

#include <QSignalSpy>
#include <QTest>
#include <utility>

#include "mockgitmanager.h"
#include "ravenlhsview.h"
#include "raventreeitem.h"

void RavenTreeTest::initTestCase() {
    auto *m_lhsView = new RavenLHSView();
    m_gitManager = new MockGitManager(qApp);
    m_tree = new RavenTree(m_lhsView);
}

void RavenTreeTest::emitStatusChangedForPayload(GitManager::status_data payload) const {
    const QSignalSpy spy{m_gitManager, &GitManager::statusChanged};
    m_gitManager->statusChanged(std::move(payload));

    QCOMPARE(spy.count(), 1);
}

void RavenTreeTest::buildTreeUncommittedNodes() const {
    // Emit status changed signal with payload
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append(
        {.path = "tmp/1", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::UNCOMMITTED, .deleted = false});

    emitStatusChangedForPayload(payload);

    // Verify tree rendered correctly
    auto model = m_tree->model();
    // Check row
    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(model->getUncommittedNode()->children.length(), 1);
    const auto uncommittedTmpNode = model->getUncommittedNode()->children.first();
    QCOMPARE(uncommittedTmpNode->name, "tmp");
    const auto childNode = uncommittedTmpNode->children.first();
    QCOMPARE(childNode->name, "1");
}

void RavenTreeTest::cleanupTestCase() const {
    qDebug() << "RavenTreeTest::cleanupTestCase called";
    m_gitManager->deleteLater();
    m_tree->deleteLater();
}

QTEST_MAIN(RavenTreeTest)
