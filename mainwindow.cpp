#include "mainwindow.h"

#include <QApplication>

#include "ravenrhsview.h"
#include "ravenstatusbar.h"

#include <QGridLayout>
#include <QSplitter>

MainWindow::MainWindow() : m_statusMessageDispatcher{new RavenStatusMessageDispatcher(this)} {
    m_git_manager = qApp->findChild<GitManager *>();
    // Window stuff
    setGeometry(0, 0, 1366, 768);
    setWindowTitle(QString("GitRaven - '%1'").arg(m_git_manager->getRepoPath()));

    // App layout stuff

    // Central widget
    auto *centralWidget = new QWidget(this);
    auto *layout = new QGridLayout;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);

    m_lhsView = new RavenLHSView(splitter);
    RavenRHSView *rhsView = new RavenRHSView(m_statusMessageDispatcher, splitter);

    splitter->addWidget(m_lhsView);
    splitter->addWidget(rhsView);

    layout->addWidget(splitter);

    splitter->setSizes({width() / 6, width() / 2});
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *statusBar = new RavenStatusBar(centralWidget);
    setStatusBar(statusBar);

    // Link status message dispatcher service to `RavenStatusBar::signalShowMessage`
    connect(m_statusMessageDispatcher, &RavenStatusMessageDispatcher::showMessage, statusBar,
            &RavenStatusBar::signalShowMessage);

    // React to app state event to run statusAsync();
    connect(qApp, &QApplication::applicationStateChanged, this, [this](const Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive)
            m_git_manager->statusAsync();
    });
}
