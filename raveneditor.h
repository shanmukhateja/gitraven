#ifndef RAVENEDITOR_H
#define RAVENEDITOR_H

#include "gitmanager.h"
#include "ravenmonaco.h"

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
    explicit RavenEditor(QWidget *parent = nullptr);

    void init();
    void updateUI();
    void openDiffItem(GitManager::GitDiffItem item);
signals:
    void signalSaveModifiedChanges(QString modified);

protected slots:
    void slotSaveModifiedChanges(QString modifiedText);

private:
    RavenMonaco *m_webEngineView;
    QVBoxLayout *m_layout;

    GitManager::GitDiffItem m_diffItem;

    bool m_diffVisible = false;
};

#endif // RAVENEDITOR_H
