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
    explicit MainWindow(GitManager* manager = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

    bool event(QEvent * e) override;

    RavenLHSView* getRavenLHSView() { return m_lhsView; }

    GitManager* getGitManager() const { return m_git_manager; }

private:
    RavenLHSView *m_lhsView = nullptr;
    GitManager *m_git_manager = nullptr;
    RavenStatusMessageDispatcher *m_statusMessageDispatcher = nullptr;
};
#endif // MAINWINDOW_H
