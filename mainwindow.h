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
    explicit MainWindow(GitManager* manager = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

    bool event(QEvent * e) override;

    RavenLHSView* getRavenLHSView() { return m_lhsView; }
    RavenStatusBar *statusBar() { return m_statusBar; }

    GitManager* getGitManager() const { return m_git_manager; }

    RavenEditor* getRavenEditor() { return m_raven_editor; }
    void setRavenEditor(RavenEditor* editor) { m_raven_editor = editor; }

private:
    RavenLHSView *m_lhsView = nullptr;
    GitManager *m_git_manager = nullptr;
    RavenEditor *m_raven_editor = nullptr;
    RavenStatusBar *m_statusBar = nullptr;
};
#endif // MAINWINDOW_H
