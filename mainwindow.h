#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ravenlhsview.h"
#include "ravenstatusmessagedispatcher.h"

#include <QMainWindow>
#include <QTreeView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    [[nodiscard]] RavenLHSView* getRavenLHSView() const { return m_lhsView; }

private:
    RavenLHSView *m_lhsView = nullptr;
    GitManager *m_git_manager = nullptr;
    RavenStatusMessageDispatcher *m_statusMessageDispatcher = nullptr;
};
#endif // MAINWINDOW_H
