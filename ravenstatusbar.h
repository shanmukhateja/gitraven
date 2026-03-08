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
public slots:
    void slotHEADChange(GitManager::GitHEADStatus status);
signals:
    void signalShowMessage(const QString msg);
private:
    void onHEADStatusButtonClicked();

    QPushButton *m_headStatusButton;
    RavenGitCheckoutDialog *m_gitCheckoutDialog;

    const int SHOW_MESSAGE_TIMEOUT_MILLIS = 2000;

};

#endif // RAVENSTATUSBAR_H
