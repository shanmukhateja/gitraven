#ifndef RAVENSTATUSBAR_H
#define RAVENSTATUSBAR_H

#include "gitmanager.h"
#include "ravengitcheckoutdialog.h"

#include <QLayout>

#include <QStatusBar>
#include <QString>

class RavenStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RavenStatusBar(QWidget *parent = nullptr);
signals:
    void signalShowMessage(const QString msg);
public slots:
    void slotHEADChange(GitManager::GitHEADStatus status);
private:
    void onHEADStatusButtonClicked();

    QPushButton *m_headStatusButton;
    RavenGitCheckoutDialog *m_gitCheckoutDialog;
    GitManager *m_gitManager;

    const int SHOW_MESSAGE_TIMEOUT_MILLIS = 2000;

};

#endif // RAVENSTATUSBAR_H
