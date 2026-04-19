//
// Created by suryateja on 19/04/26.
//

#ifndef GITRAVEN_TEST_RAVENTREE_H
#define GITRAVEN_TEST_RAVENTREE_H

#include "raventree.h"

class RavenTreeTest : public QObject {
    Q_OBJECT
    RavenTree *m_tree = nullptr;
    GitManagerBase *m_gitManager = nullptr;

  private slots:
    void initTestCase();

    void emitStatusChangedForPayload(GitManager::status_data) const;

    void buildTreeUncommittedNodes() const;

    void cleanupTestCase() const;
};

#endif // GITRAVEN_TEST_RAVENTREE_H
