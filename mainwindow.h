#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "raveneditor.h"
#include "ravenlhsview.h"
#include "ravenstatusbar.h"

#include <QMainWindow>
#include <QTreeView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool event(QEvent * e) override;

    RavenLHSView* getRavenLHSView() { return m_lhsView; }
    RavenStatusBar *statusBar() { return m_statusBar; }

    GitManager* getGitManager() { return m_git_manager; }
    void setGitManager(GitManager* manager) { m_git_manager = manager; }
    RavenEditor* getRavenEditor() { return m_raven_editor; }
    void setRavenEditor(RavenEditor* editor) { m_raven_editor = editor; }

private:
    RavenLHSView *m_lhsView;
    GitManager *m_git_manager;
    RavenEditor *m_raven_editor;
    RavenStatusBar *m_statusBar;
};
#endif // MAINWINDOW_H
