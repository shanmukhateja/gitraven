#include "ravenstatusbar.h"

#include <QLabel>

RavenStatusBar::RavenStatusBar(QWidget *parent)
    : m_statusBar(new QStatusBar(this)),
    m_layout(new QHBoxLayout(this))
{
    setLayout(m_layout);
    init();
}

void RavenStatusBar::init()
{
    connect(this, &RavenStatusBar::signalHEADChange, this, &RavenStatusBar::slotHEADChange);
    auto branchLabel = new QLabel("Current HEAD:");
    m_headStatusButton = new QPushButton("");
    m_layout->addWidget(branchLabel, 0, Qt::AlignLeft);
    m_layout->addWidget(m_headStatusButton, 0, Qt::AlignLeft);
    m_layout->addWidget(new QLabel(), 1, Qt::AlignLeft);
}

void RavenStatusBar::slotHEADChange(GitManager::GitHEADStatus status)
{
    qDebug() << "RavenStatusBar::slotBranchNameChange called with HEAD name=" << status.name << "type=" << status.type;

    // Compute name & type here
    QString name   = status.type == GitManager::GIT_HEAD_TYPE_BRANCH ? status.name : status.name.first(7);
    QIcon   icon   = status.type == GitManager::GIT_HEAD_TYPE_BRANCH ? QIcon::fromTheme("vcs-branch") : QIcon::fromTheme("vcs-commit");
    int     width  = status.type == GitManager::GIT_HEAD_TYPE_BRANCH ? m_headStatusButton->width() : m_headStatusButton->width() / 0.6;

    // Ellipses affect when needed.
    QFontMetrics fm(m_headStatusButton->font());
    QString elidedText = fm.elidedText(name, Qt::ElideRight, width);

    // Set text & icon
    m_headStatusButton->setText(elidedText);
    m_headStatusButton->setIcon(icon);
    m_headStatusButton->setToolTip(status.name);
}
