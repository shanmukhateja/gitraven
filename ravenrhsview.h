#ifndef RAVENRHSVIEW_H
#define RAVENRHSVIEW_H

#include "gitmanager.h"
#include "mainwindow.h"
#include "raveneditor.h"

#include <QVBoxLayout>
#include <QWidget>

class RavenRHSView : public QWidget
{
    Q_OBJECT
public:
    explicit RavenRHSView(QWidget *parent);
    ~RavenRHSView() override;

    void initLandingInfo();

public slots:
    void updateUI(std::optional<GitManager::GitDiffItem> item);

private:
    bool m_showLandingInfo = true;

    MainWindow *m_mainWindow;
    RavenTree   *m_ravenTree;
    RavenEditor *m_ravenEditor;

    QWidget *m_landingInfoWidget;
};

#endif // RAVENRHSVIEW_H
