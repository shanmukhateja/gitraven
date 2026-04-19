#ifndef RAVENEDITOR_H
#define RAVENEDITOR_H

#include "gitmanager.h"
#include "ravenmonaco.h"
#include "ravenstatusmessagedispatcher.h"

#include <QEvent>
#include <QObject>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>
#include <QWebEngineView>

class RavenEditor : public QWidget {
    Q_OBJECT
  public:
    explicit RavenEditor(RavenStatusMessageDispatcher* statusMsgDispatcher, QWidget* parent = nullptr);
    ~RavenEditor();

    void init();
    void updateUI();
    void openDiffItem(GitManager::GitDiffItem item);
  signals:
    void signalSaveModifiedChanges(QString modified);

  protected slots:
    void slotSaveModifiedChanges(QString modifiedText);

  private:
    RavenMonaco* m_webEngineView = nullptr;
    RavenStatusMessageDispatcher* m_statusMsgDispatcher = nullptr;
    QVBoxLayout* m_layout = nullptr;

    GitManager::GitDiffItem m_diffItem;

    bool m_initFinished = false;
};

#endif // RAVENEDITOR_H
