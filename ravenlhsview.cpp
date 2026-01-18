#include "ravenlhsview.h"

#include "mainwindow.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

RavenLHSView::RavenLHSView(QWidget *parent)
    : QWidget(parent),
    m_mainWindow(static_cast<MainWindow*>(topLevelWidget()->window())),
    m_treeView(new RavenTree(this)),
    m_maxStatusFileCountWarningLabel(new QLabel(this))
{
    // Widget config
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    // Increases QTreeView height against commit message UI
    layout->setStretch(0, 1);

    // Commit message UI
    QLabel *commitMessageHeading = new QLabel(this);
    commitMessageHeading->setText("Commit Message");
    layout->addWidget(commitMessageHeading);

    // Commit message text box
    m_commitMessageTextBox = new QPlainTextEdit(this);
    m_commitMessageTextBox->setTabChangesFocus(true);
    layout->addWidget(m_commitMessageTextBox);

    // Commit button & "ammend commit" checkbox
    auto commitOrAmendWidget = new QWidget(this);
    auto commitOrAmmendLayout = new QHBoxLayout(commitOrAmendWidget);
    commitOrAmmendLayout->setContentsMargins(0, 0, 0, 0);

    m_amendCommitCheckbox = new QCheckBox(commitOrAmendWidget);
    m_amendCommitCheckbox->setText("Amend commit");

    m_commitMessageButton = new QPushButton(commitOrAmendWidget);
    m_commitMessageButton->setText("Commit");

    commitOrAmmendLayout->addWidget(m_amendCommitCheckbox);
    commitOrAmmendLayout->addWidget(m_commitMessageButton);

    layout->addWidget(commitOrAmendWidget);

    // connect listeners
    connect(m_treeView->model(), &RavenTreeModel::dataChanged, this, &RavenLHSView::updateCommitMessageUI);
    connect(m_commitMessageButton,&QPushButton::clicked, this, &RavenLHSView::commit);

    // Max status file size banner
    connect(this, &RavenLHSView::signalMaxStatusFileCountReached, this, &RavenLHSView::slotMaxStatusFileCountReached);
}

void RavenLHSView::updateCommitMessageUI()
{
    // qDebug() << "RavenLHSView::updateCommitMessageUI called";
    auto model = m_treeView->model();
    auto stagingEmpty = model->isStagingEmpty();
    updateCommitMessageUIState(!stagingEmpty);
}

void RavenLHSView::slotMaxStatusFileCountReached(bool reached)
{
    qDebug() << "RavenLHSView::slotMaxStatusFileCountReached called with reached=" << reached;
    // Show message in UI
    this->showMaxStatusFileCountWarning = reached;
    // Update layout if required
    this->isMaxStatusFileCountWarningVisible = layout()->children().contains(this->m_maxStatusFileCountWarningLabel);
    if (!this->isMaxStatusFileCountWarningVisible) this->buildMaxStatusFileCountWarningUI();
}

void RavenLHSView::buildMaxStatusFileCountWarningUI() {
    QString msg;
    // FIXME: Use `QIcon` for warning icon.
    msg.append("⚠️ Large number of files detected, showing first ");
    msg.append(QString::number(m_treeView->MAX_STATUS_FILES_COUNT));
    msg.append(" items.");
    this->m_maxStatusFileCountWarningLabel->setText(msg);
    this->m_maxStatusFileCountWarningLabel->setStyleSheet("background-color:yellow;color:black;padding:5px");
    this->m_maxStatusFileCountWarningLabel->setWordWrap(true);
    ((QVBoxLayout*)layout())->insertWidget(0, m_maxStatusFileCountWarningLabel);
}

QList<QString> RavenLHSView::getAllStagingItemAbsPaths()
{
    QList<QString> paths = {};

    auto gitManager = m_mainWindow->getGitManager();
    auto statusResponse = gitManager->status(false);

    foreach (auto item, statusResponse.statusItems) {
        if (item.category == RavenTreeItem::BOTH || item.category == RavenTreeItem::STAGING)
        {
            qDebug() << "staging item=" << item.path;
            paths.append(item.path);
        }
    }

    return paths;
}

void RavenLHSView::updateCommitMessageUIState(bool state)
{
    // qDebug() << "RavenLHSView::updateCommitMessageUIState with state=" << state;
    m_commitMessageTextBox->setEnabled(state);
    m_commitMessageButton->setEnabled(state);
    m_amendCommitCheckbox->setEnabled(state);
}

void RavenLHSView::commit()
{
    auto gitManager = m_mainWindow->getGitManager();

    // Extract absPaths from all children of staging root node.
    QList<QString> paths = getAllStagingItemAbsPaths();

    QString msg = m_commitMessageTextBox->toPlainText();

    // Check amend condition
    bool amend = m_amendCommitCheckbox->isChecked();

    int result = gitManager->commit(paths, msg, amend);
    if (result == 0)
    {
        // update UI
        resetCommitMessageUI();
        gitManager->status();
    }
    else
    {
        // Failed to commit changes
        QMessageBox msg(QMessageBox::Critical, "Error", "Failed to commit staged changes. Please check the logs for more information.", QMessageBox::Ok, this);
        msg.exec();
    }
}

void RavenLHSView::resetCommitMessageUI()
{
    m_commitMessageTextBox->setPlainText("");
    m_amendCommitCheckbox->setChecked(false);
}
