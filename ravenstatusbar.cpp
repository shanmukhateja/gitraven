#include "ravenstatusbar.h"
#include "mainwindow.h"

#include <QLabel>

RavenStatusBar::RavenStatusBar(QWidget *parent)
    :   QStatusBar{parent}
{
    m_gitManager = qApp->findChild<GitManager *>();
    m_gitCheckoutDialog = new RavenGitCheckoutDialog(m_gitManager, this);

    auto branchLabel = new QLabel("Current HEAD:", this);
    m_headStatusButton = new QPushButton("", this);
    m_headStatusButton->setDisabled(true);
    connect(m_headStatusButton, &QPushButton::clicked, this, &RavenStatusBar::onHEADStatusButtonClicked);
    
    layout()->addWidget(branchLabel);
    layout()->addWidget(m_headStatusButton);
    layout()->addWidget(new QLabel(this));

    // Change status bar's checkout ref name when GitManager::status is updated
    connect(
        m_gitManager,
        &GitManager::statusChanged,
        this,
        [this] {
            slotHEADChange(m_gitManager->findHEADStatus());
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

void RavenStatusBar::slotHEADChange(GitManager::GitHEADStatus status)
{
    qDebug() << "RavenStatusBar::slotBranchNameChange called with HEAD name=" << status.name << "type=" << status.type;

    // Compute name & type here
    QString name;
    QIcon icon;
    double width;
    // Ellipses affect on name.
    QFontMetrics fm(m_headStatusButton->font());

    Q_ASSERT(!status.name.isEmpty());
    switch (status.type) {
        case GitManager::GIT_HEAD_TYPE_COMMIT:
        width = m_headStatusButton->width() / 0.6;
        name = fm.elidedText(status.name.first(7), Qt::ElideRight, static_cast<int>(width));
        icon = QIcon::fromTheme("vcs-commit");
        break;
        case GitManager::GIT_HEAD_TYPE_BRANCH:
        name = status.name;
        icon = QIcon::fromTheme("vcs-branch");
        break;
    case GitManager::GIT_HEAD_TYPE_TAG:
        name = status.name;
        icon = QIcon::fromTheme("tag-symbolic");
        break;
    }

    // Update button
    m_headStatusButton->setDisabled(false);
    m_headStatusButton->setText(name);
    m_headStatusButton->setIcon(icon);
    m_headStatusButton->setToolTip(QString("%1\n\nClick to checkout to different branch/tag.").arg(status.name));
}
