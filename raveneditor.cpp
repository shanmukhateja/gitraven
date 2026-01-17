#include "raveneditor.h"
#include "ravenutils.h"
#include "ravenmonacopage.h"

RavenEditor::RavenEditor(RavenStatusMessageDispatcher *statusMsgDispatcher, QWidget *parent)
    : QWidget{parent},
    m_statusMsgDispatcher(statusMsgDispatcher)
{}

void RavenEditor::init()
{
    auto layout = new QVBoxLayout(this);
    m_webEngineView = new RavenMonaco(this);
    layout->addWidget(m_webEngineView);

    // EVENT LISTENERS

    // Update Monaco with diff but wait for page to finish loading monaco
    // This is needed for first-time init of RavenMonaco.
    connect(m_webEngineView->page(), &RavenMonacoPage::signalInitFinished, this, &RavenEditor::updateUI);
    // Listen for Ctrl+S and save modified file content to disk.
    connect(this, &RavenEditor::signalSaveModifiedChanges, this, &RavenEditor::slotSaveModifiedChanges);

    m_initFinished = true;
}

void RavenEditor::updateUI()
{
    auto *page = m_webEngineView->page();

    // Update text content
    page->updateText(m_diffItem);

    // update editor readonly status
    // Note: We don't allow "staged" items to be modified any further.
    bool readonly = m_diffItem.category == RavenTreeItem::STAGING;
    page->setReadonly(readonly);
}

void RavenEditor::openDiffItem(GitManager::GitDiffItem item)
{
    qDebug() << "RavenEditor::openDiffItem called";

    // Update diff item ref.
    m_diffItem = item;

    // Init & render diff
    // 1. Check if we have initialized.
    bool initFinished = m_initFinished;
    if (initFinished)
    {
        // 2. Next, we check if page is initialized.
        initFinished = m_webEngineView->isInitFinished();
    }
    initFinished ? updateUI() : init();
}

void RavenEditor::slotSaveModifiedChanges(QString modifiedText)
{
    qDebug() << "RavenEditor::slotSaveModifiedChanges(QString modifiedText) called!!!";

    auto filePath = m_diffItem.newFilePath;
    qDebug() << "newPath=" << filePath;
    // qDebug() << "text   =" << modifiedText;

    bool result = RavenUtils::saveFile(filePath, modifiedText);

    // Show status
    m_statusMsgDispatcher->showMessage(result ? "File changes saved successfully" : "Failed to save changes");

    // TODO: Update Diff UI
    // Note:    This will reload the diff viewer UI so user can see
    //          what was actually written to disk.
}
