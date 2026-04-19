#include "test_gitmanager.h"
#include <QDebug>
#include <QTest>

void GitManagerTest::initTestCase() {}
void GitManagerTest::testStatus() { qDebug() << "testStatus"; }
void GitManagerTest::cleanupTestCase() { qDebug() << "cleanupTestCase"; }

QTEST_MAIN(GitManagerTest)
