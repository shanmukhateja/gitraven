#include <QtTest>

#include "test_raventree.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    int status = 0;

    {
        RavenTreeTest tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}
