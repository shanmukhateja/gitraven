#ifndef RAVENMONACO_H
#define RAVENMONACO_H

#include "ravenmonacopage.h"
#include "ravenmonacohttpserver.h"
#include "ravenmonacobridge.h"

#include <QWebEngineView>
#include <QWidget>

#include <QJsonObject>

class RavenMonaco : public QWebEngineView
{
    Q_OBJECT
public:
    explicit RavenMonaco(QWidget *parent = nullptr);

    RavenMonacoPage *page() const { Q_ASSERT(m_page); return m_page; }

    bool isInitFinished() {
        auto result = m_page != nullptr;
        if (result)
        {
            result = m_page->isInitFinished();
        }
        return result;
    }
private:
    RavenMonacoPage *m_page;
    RavenMonacoHTTPServer *m_server;
    RavenMonacoBridge *m_bridge;
    QWebChannel *m_channel;
};

#endif // RAVENMONACO_H
