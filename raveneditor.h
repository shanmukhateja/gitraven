#ifndef RAVENEDITOR_H
#define RAVENEDITOR_H

#include "gitmanager.h"
#include "ravenmonaco.h"
#include "ravenstatusmessagedispatcher.h"

#include <QObject>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QEvent>

class RavenEditor : public QWidget
{
    Q_OBJECT
public:
    explicit RavenEditor(RavenStatusMessageDispatcher *statusMsgDispatcher, QWidget *parent = nullptr);

    void init();
    void updateUI();
    void openDiffItem(GitManager::GitDiffItem item);
signals:
    void signalSaveModifiedChanges(QString modified);

protected slots:
    void slotSaveModifiedChanges(QString modifiedText);

private:
    RavenMonaco *m_webEngineView;
    RavenStatusMessageDispatcher *m_statusMsgDispatcher;

    GitManager::GitDiffItem m_diffItem;

    bool m_initFinished = false;
};

#endif // RAVENEDITOR_H
