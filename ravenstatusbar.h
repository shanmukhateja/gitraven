#ifndef RAVENSTATUSBAR_H
#define RAVENSTATUSBAR_H

#include "gitmanager.h"

#include <QObject>

#include <QHBoxLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

class RavenStatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit RavenStatusBar(QWidget *parent = nullptr);
signals:
    void signalHEADChange(GitManager::GitHEADStatus branchName);
public slots:
    void slotHEADChange(GitManager::GitHEADStatus branchName);

private:
    void init();

    QHBoxLayout *m_layout;
    QStatusBar *m_statusBar;
    QPushButton *m_headStatusButton;
};

#endif // RAVENSTATUSBAR_H
