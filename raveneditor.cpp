#include "raveneditor.h"

#include "ravenutils.h"

RavenEditor::RavenEditor(QWidget *parent)
    : QWidget{parent},
    m_webEngineView(new RavenMonaco(this)),
    m_layout(new QVBoxLayout(this))
{
    setLayout(m_layout);
    init();

    connect(this, &RavenEditor::signalSaveModifiedChanges, this, &RavenEditor::slotSaveModifiedChanges);
}

void RavenEditor::init()
{
    // Hide the widget from UI
    m_webEngineView->hide();
}

void RavenEditor::updateUI()
{
    if (!m_diffVisible) {
        m_layout->addWidget(m_webEngineView);
        m_layout->setContentsMargins(0, 0, 0, 0);
        m_webEngineView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_webEngineView->show();

        m_diffVisible = true;
    }
}

void RavenEditor::openDiffItem(GitManager::GitDiffItem item)
{
    qDebug() << "RavenEditor::openDiffItem called";

    // Update diff item ref.
    m_diffItem = item;

    // Refresh UI
    updateUI();

    auto *page = m_webEngineView->page();

    // update editor readonly status
    // Note: We don't allow "staged" items to be modified any further.
    bool readonly = item.category == RavenTreeItem::STAGING;
    page->setReadonly(readonly);

    // Update text content
    page->updateText(item);
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
