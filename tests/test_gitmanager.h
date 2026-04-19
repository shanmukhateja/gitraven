#ifndef GITRAVEN_TEST_GITMANAGER_H
#define GITRAVEN_TEST_GITMANAGER_H

#include <QObject>

class GitManagerTest : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase();
    void testStatus();
    void cleanupTestCase();
};

#endif // GITRAVEN_TEST_GITMANAGER_H