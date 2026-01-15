#ifndef RAVENSTATUSBAR_H
#define RAVENSTATUSBAR_H

#include "gitmanager.h"
#include "ravengitcheckoutdialog.h"

#include <QObject>
#include <QLayout>

#include <QPushButton>
#include <QStatusBar>
#include <QString>

class RavenStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RavenStatusBar(QWidget *parent = nullptr);
signals:
    void signalHEADChange(GitManager::GitHEADStatus branchName);
public slots:
    void slotHEADChange(GitManager::GitHEADStatus branchName);

private:
    void onHEADStatusButtonClicked();

    QPushButton *m_headStatusButton;
    RavenGitCheckoutDialog *m_gitCheckoutDialog;
};

#endif // RAVENSTATUSBAR_H
