#include "ravenmonacobridge.h"

#include "raveneditor.h"

RavenMonacoBridge::RavenMonacoBridge(QObject *parent, RavenEditor *editor)
    : QObject{parent}
{
    m_ravenEditor = editor;
}

/**
 * @brief This function is called by Monaco when user has modified file contents.
 * @param modifiedText - The modified text contents from Monaco side.
 */
void RavenMonacoBridge::saveModifiedChanges(QString modifiedText)
{
    qDebug() << "RavenMonacoBridge::saveModifiedChanges called";

    emit m_ravenEditor->signalSaveModifiedChanges(modifiedText);
}

