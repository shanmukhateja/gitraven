#include "ravenstatusbar.h"
#include "mainwindow.h"

#include <QLabel>

RavenStatusBar::RavenStatusBar(QWidget *parent)
    :   QStatusBar{parent}
{
    // Note:    `this` is not a reliable parent at this moment.
    //          We cannot move this logic inside `RavenGitCheckoutDialog` constructor
    auto window = (qobject_cast<MainWindow *>(parent->window()));
    m_gitCheckoutDialog = new RavenGitCheckoutDialog(window->getGitManager(), this);

    m_headStatusButton = new QPushButton("");
    m_headStatusButton->setDisabled(true);
    connect(m_headStatusButton, &QPushButton::clicked, this, &RavenStatusBar::onHEADStatusButtonClicked);
    layout()->addWidget(m_headStatusButton);

    // Change status bar's checkout ref name when GitManager::status is called
    connect(
        window->getGitManager(),
        &GitManager::statusChanged,
        this,
        [this](GitManager::status_data sd)
        {
            onCheckoutRefLabelChange(sd.headStatus);
        }
    );

    connect(this, &RavenStatusBar::signalShowMessage, this, [this](const QString msg) {
        this->showMessage(msg, SHOW_MESSAGE_TIMEOUT_MILLIS);
    });
}

void RavenStatusBar::onHEADStatusButtonClicked()
{
    qDebug() << "RavenStatusBar::onHEADStatusButtonClicked called";
    emit m_gitCheckoutDialog->signalOnBranchChangeRequested();
}

void RavenStatusBar::onCheckoutRefLabelChange(GitManager::GitHEADStatus status)
{
    qDebug() << "RavenStatusBar::slotBranchNameChange called with HEAD name=" << status.name << "type=" << status.type;

    // Compute name & type here
    QString name;
    QIcon icon;
    int width;
    // Ellipses affect on name.
    QFontMetrics fm(m_headStatusButton->font());

    Q_ASSERT(!status.name.isEmpty());
    switch (status.type) {
        case GitManager::GIT_HEAD_TYPE_COMMIT:
        name = fm.elidedText(status.name.first(7), Qt::ElideRight, width);
        icon = QIcon::fromTheme("vcs-commit");
        width = m_headStatusButton->width() / 0.6;
        break;
        case GitManager::GIT_HEAD_TYPE_BRANCH:
        name = status.name;
        icon = QIcon::fromTheme("vcs-branch");
        width = m_headStatusButton->width();
        break;
    case GitManager::GIT_HEAD_TYPE_TAG:
        name = status.name;
        icon = QIcon::fromTheme("tag-symbolic");
        width = m_headStatusButton->width();
        break;
    }

    // Update button
    m_headStatusButton->setDisabled(false);
    m_headStatusButton->setText(name);
    m_headStatusButton->setIcon(icon);
    m_headStatusButton->setToolTip(QString("%1\n\nClick to checkout to different branch/tag.").arg(status.name));
}
