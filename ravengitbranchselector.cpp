#include "ravengitbranchselector.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

RavenGitBranchSelector::RavenGitBranchSelector(GitManager *manager, QWidget* parent)
    : QWidget{parent},
    m_gitManager(manager)
{
    connect(this, &RavenGitBranchSelector::signalOnBranchChangeRequested, this, &RavenGitBranchSelector::slotOnBranchChangeRequested);
}

void RavenGitBranchSelector::slotOnBranchChangeRequested()
{
    qDebug() << "RavenGitBranchSelector::handleOnBranchChangeRequested() called";

    // Fetch all branches/tags
    m_gitManager->getAllBranchesAndTags();

    QDialog *dialog = new QDialog(this);
    dialog->setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    layout->addWidget(new QLabel("Select a Git branch/tag to checkout."));

    dialog->setLayout(layout);
    dialog->show();
}
