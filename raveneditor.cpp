#include "raveneditor.h"

#include "ravenutils.h"

RavenEditor::RavenEditor(QWidget *parent)
    : QWidget{parent}
{}

void RavenEditor::init()
{
    auto layout = new QVBoxLayout(this);
    m_webEngineView = new RavenMonaco(this);
    layout->addWidget(m_webEngineView);
    m_webEngineView->focusWidget();
    m_diffVisible = true;

    // EVENT LISTENERS

    // Wait for page to finish loading before updating Monaco state.
    connect(m_webEngineView->page(), &QWebEnginePage::loadFinished, this, &RavenEditor::updateUI);
    // Listen for Ctrl+S and save modified file content to disk.
    connect(this, &RavenEditor::signalSaveModifiedChanges, this, &RavenEditor::slotSaveModifiedChanges);
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

    // Setup UI for first time
    if (!m_diffVisible) init();

    // Update Monaco state.
    updateUI();
}

void RavenEditor::slotSaveModifiedChanges(QString modifiedText)
{
    qDebug() << "RavenEditor::slotSaveModifiedChanges(QString modifiedText) called!!!";

    auto filePath = m_diffItem.newFilePath;
    qDebug() << "newPath=" << filePath;
    qDebug() << "text   =" << modifiedText;

    RavenUtils::saveFile(filePath, modifiedText);

    // TODO: show status
}
