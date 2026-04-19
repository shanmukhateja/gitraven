#ifndef RAVENGITCHECKOUTDIALOG_H
#define RAVENGITCHECKOUTDIALOG_H

#include "gitmanager.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>

class RavenGitCheckoutDialog : public QDialog {
    Q_OBJECT
  public:
    RavenGitCheckoutDialog(GitManager* manager, QWidget* parent = nullptr);
  signals:
    void signalOnBranchChangeRequested();

  public slots:
    void slotOnBranchChangeRequested();

  protected:
    void closeEvent(QCloseEvent* e) override;

  private:
    GitManager* m_gitManager;

    // Create new layout with dialog as parent to "own" the layout.
    QVBoxLayout* mainLayout;
    QLabel* m_checkoutStatusLabel;
    QLineEdit* m_searchList;
    QListWidget* m_list;
    QPushButton* m_checkoutButton;

    // Cache icons here
    QIcon m_branchIconLocal = QIcon::fromTheme("branch-symbolic");
    QIcon m_branchIconRemote = QIcon::fromTheme("vcs-branch-symbolic");
    ;
    QIcon m_tagIcon = QIcon::fromTheme("tag-symbolic");

    void resetCheckoutWarningLabel();
};

#endif // RAVENGITCHECKOUTDIALOG_H
