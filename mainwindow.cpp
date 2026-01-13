#include "mainwindow.h"
#include "ravenlhsview.h"
#include "ravenrhsview.h"

#include <QEvent>
#include <QGridLayout>
#include <QSplitter>

#include <QLabel>


MainWindow::MainWindow(GitManager *manager, QWidget *parent)
    : QMainWindow(parent),
    m_git_manager{manager}
{
    // Window stuff
    setGeometry(0,0, 1366, 768);
    setWindowTitle("GitRaven");

    // App layout stuff

    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);

    m_lhsView = new RavenLHSView(centralWidget);
    RavenRHSView *rhsView = new RavenRHSView(centralWidget);

    splitter->addWidget(m_lhsView);
    splitter->addWidget(rhsView);

    QGridLayout *layout = new QGridLayout(centralWidget);
    layout->addWidget(splitter);

    splitter->setSizes({width() / 6, width() / 2});
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_statusBar = new RavenStatusBar(centralWidget);
    layout->addWidget(m_statusBar);
}

MainWindow::~MainWindow()
{
    delete m_git_manager;
}

// https://forum.qt.io/post/424408
bool MainWindow::event(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::WindowActivate:
        // refresh git status
        // Note: This function is also called on first init.
        getGitManager()->status();
        break;
    default:
        break;
    }
    return QMainWindow::event(e);
}
