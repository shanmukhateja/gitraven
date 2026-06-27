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
    emit m_gitManager->statusChanged(std::move(payload));

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

void RavenTreeTest::buildTreeStagingNodes() const {
    // Emit status changed signal with payload
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append({.path = "staging/1/staging/2.txt",
                                .flag = GIT_STATUS_INDEX_NEW,
                                .category = RavenTreeItem::STAGING,
                                .deleted = false});
    payload.statusItems.append({.path = "staging1/2/staging/24.txt",
                                .flag = GIT_STATUS_WT_NEW,
                                .category = RavenTreeItem::STAGING,
                                .deleted = true});

    emitStatusChangedForPayload(payload);

    // Verify tree rendered correctly
    auto model = m_tree->model();
    // Check rows
    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(model->getStagingNode()->children.length(), 2);
    auto children = model->getStagingNode()->children;
    // first row
    const auto firstChildOfSectionNode = children.first();
    QCOMPARE(firstChildOfSectionNode->name, "staging");
    auto childNode = firstChildOfSectionNode->children.first();
    QCOMPARE(childNode->name, "1");
    childNode = childNode->children.first();
    QCOMPARE(childNode->name, "staging");
    childNode = childNode->children.first();
    QCOMPARE(childNode->name, "2.txt");
    // second row
    const auto secondChildOfSectionNode = children.at(1);
    QCOMPARE(secondChildOfSectionNode->name, "staging1");
    childNode = secondChildOfSectionNode->children.first();
    QCOMPARE(childNode->name, "2");
    childNode = childNode->children.first();
    QCOMPARE(childNode->name, "staging");
    childNode = childNode->children.first();
    QCOMPARE(childNode->name, "24.txt");
}

void RavenTreeTest::buildTreeDuplicatePaths() const {
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append(
        {.path = "dup/file.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});
    payload.statusItems.append(
        {.path = "dup/file.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});

    emitStatusChangedForPayload(payload);

    auto model = m_tree->model();
    QCOMPARE(model->getStagingNode()->children.length(), 1);

    const auto dirNode = model->getStagingNode()->children.first();
    QCOMPARE(dirNode->name, "dup");
    QCOMPARE(dirNode->children.length(), 1); // should NOT duplicate
}

void RavenTreeTest::buildTreeSiblingFiles() const {
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append(
        {.path = "dir/a.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});
    payload.statusItems.append(
        {.path = "dir/b.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});

    emitStatusChangedForPayload(payload);

    auto children = m_tree->model()->getStagingNode()->children;
    QCOMPARE(children.length(), 1);

    const auto dirNode = children.first();
    QCOMPARE(dirNode->children.length(), 2);
}

void RavenTreeTest::buildTreeMixedDepth() const {
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append(
        {.path = "a.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::UNCOMMITTED, .deleted = false});
    payload.statusItems.append({.path = "dir/sub/file.txt",
                                .flag = GIT_STATUS_INDEX_NEW,
                                .category = RavenTreeItem::UNCOMMITTED,
                                .deleted = false});

    emitStatusChangedForPayload(payload);

    auto children = m_tree->model()->getUncommittedNode()->children;
    QCOMPARE(children.length(), 2);
}

void RavenTreeTest::buildTreeSamePathMultipleStatuses() const {
    GitManager::status_data payload = {.statusItems = {}};

    payload.statusItems.append(
        {.path = "file.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});

    payload.statusItems.append(
        {.path = "file.txt", .flag = GIT_STATUS_WT_MODIFIED, .category = RavenTreeItem::UNCOMMITTED, .deleted = false});

    emitStatusChangedForPayload(payload);

    auto model = m_tree->model();

    // Should appear in BOTH sections
    QCOMPARE(model->getStagingNode()->children.length(), 1);
    QCOMPARE(model->getUncommittedNode()->children.length(), 1);
}

void RavenTreeTest::buildTreeSharedDirectories() const {
    GitManager::status_data payload = {.statusItems = {}};

    payload.statusItems.append(
        {.path = "dir/a.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});

    payload.statusItems.append(
        {.path = "dir/b.txt", .flag = GIT_STATUS_INDEX_NEW, .category = RavenTreeItem::STAGING, .deleted = false});

    emitStatusChangedForPayload(payload);

    auto dirNode = m_tree->model()->getStagingNode()->children.first();

    QCOMPARE(dirNode->name, "dir");
    QCOMPARE(dirNode->children.length(), 2);
}

void RavenTreeTest::buildTreeDeletedItems() const {
    GitManager::status_data payload = {.statusItems = {}};
    payload.statusItems.append({.path = "deleted/file.txt",
                                .flag = GIT_STATUS_WT_DELETED,
                                .category = RavenTreeItem::UNCOMMITTED,
                                .deleted = true});

    emitStatusChangedForPayload(payload);

    auto node = m_tree->model()->getUncommittedNode()->children.first()->children.first();

    QCOMPARE(node->name, "file.txt");
    QCOMPARE(node->deleted, true);
}

void RavenTreeTest::buildTreeModifiedInBoth() const {
    GitManager::status_data payload = {.statusItems = {}};

    payload.statusItems.append({.path = "file.txt",
                                .flag = static_cast<git_status_t>(GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_WT_MODIFIED),
                                .category = RavenTreeItem::BOTH,
                                .deleted = false});

    emitStatusChangedForPayload(payload);

    const auto model = m_tree->model();

    // EXPECTATION: appears in both sections
    QCOMPARE(model->getStagingNode()->children.length(), 1);
    QCOMPARE(model->getUncommittedNode()->children.length(), 1);
}

void RavenTreeTest::treeStagingOnly() const {
    GitManager::GitStatusItem item = {.path = "a.txt",
                                      .flag = GIT_STATUS_INDEX_NEW,
                                      .category = RavenTreeItem::RavenTreeCategory::STAGING,
                                      .deleted = false};

    emitStatusChangedForPayload({.statusItems = {item}});

    QCOMPARE(m_tree->model()->getStagingNode()->children.length(), 1);
    QCOMPARE(m_tree->model()->getUncommittedNode()->children.length(), 0);
}

void RavenTreeTest::buildTreeMixedCategoriesSameDir() const {
    GitManager::status_data payload = {.statusItems = {}};

    payload.statusItems.append({.path = "dir/a.txt", .category = RavenTreeItem::STAGING});
    payload.statusItems.append({.path = "dir/a.txt", .category = RavenTreeItem::UNCOMMITTED});
    payload.statusItems.append({.path = "dir/b.txt", .category = RavenTreeItem::STAGING});

    emitStatusChangedForPayload(payload);

    QCOMPARE(m_tree->model()->getStagingNode()->children.first()->children.length(), 2);
    QCOMPARE(m_tree->model()->getUncommittedNode()->children.first()->children.length(), 1);
}

void RavenTreeTest::buildTreePathNormalization() const {
    GitManager::status_data payload = {.statusItems = {}};

    payload.statusItems.append({.path = "dir//a.txt", .category = RavenTreeItem::STAGING});
    payload.statusItems.append({.path = "dir/a.txt", .category = RavenTreeItem::STAGING});

    emitStatusChangedForPayload(payload);

    QCOMPARE(m_tree->model()->getStagingNode()->children.first()->children.length(), 1);
}

void RavenTreeTest::cleanupTestCase() {
    qDebug() << "RavenTreeTest::cleanupTestCase called";
    delete m_gitManager;
    delete m_tree;

    m_gitManager = nullptr;
    m_tree = nullptr;
}
