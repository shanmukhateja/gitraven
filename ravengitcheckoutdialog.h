#ifndef RAVENGITCHECKOUTDIALOG_H
#define RAVENGITCHECKOUTDIALOG_H

#include "gitmanager.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

class RavenGitCheckoutDialog : public QDialog
{
    Q_OBJECT
public:
    RavenGitCheckoutDialog(GitManager *manager, QWidget *parent = nullptr);
signals:
    void signalOnBranchChangeRequested();

public slots:
    void slotOnBranchChangeRequested();

private:
    GitManager *m_gitManager;

    QLabel *m_checkoutStatusLabel;
    QLineEdit *m_searchList;
    QListWidget *m_list;
    QPushButton *m_checkoutButton;

    void resetCheckoutWarningLabel();
};

#endif // RAVENGITCHECKOUTDIALOG_H
