#ifndef RAVENMONACO_H
#define RAVENMONACO_H

#include "ravenmonacopage.h"
#include "ravenmonacohttpserver.h"
#include "ravenmonacobridge.h"

#include <QWebEngineView>
#include <QWidget>
#include <qevent.h>

#include <QJsonObject>

class RavenMonaco : public QWebEngineView
{
    Q_OBJECT
public:
    explicit RavenMonaco(QWidget *parent = nullptr);

    RavenMonacoPage *page() const { return m_page; }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        // Ensure that the web view resizes dynamically when the parent widget is resized
        resize(event->size()); // Resize webView to match the parent widget
        QWidget::resizeEvent(event);
    }
    void setDefaultUrl();

public slots:
    void setTheme(Qt::ColorScheme colorScheme);
private:
    RavenMonacoPage *m_page;
    RavenMonacoHTTPServer *m_server;
    RavenMonacoBridge *m_bridge;
    QWebChannel *m_channel;
};

#endif // RAVENMONACO_H
