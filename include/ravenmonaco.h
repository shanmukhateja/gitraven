#ifndef RAVENMONACO_H
#define RAVENMONACO_H

#include "ravenmonacobridge.h"
#include "ravenmonacohttpserver.h"
#include "ravenmonacopage.h"

#include <QWebEngineView>
#include <QWidget>

#include <QJsonObject>

class RavenMonaco : public QWebEngineView {
    Q_OBJECT
  public:
    explicit RavenMonaco(QWidget* parent = nullptr);
    ~RavenMonaco() override;

    RavenMonacoPage* page() const {
        Q_ASSERT(m_page);
        return m_page;
    }

    bool isInitFinished() {
        auto result = m_page != nullptr;
        if (result) {
            result = m_page->isInitFinished();
        }
        return result;
    }

  private:
    RavenMonacoPage* m_page = nullptr;
    RavenMonacoHTTPServer* m_server = nullptr;
    RavenMonacoBridge* m_bridge = nullptr;
    QWebChannel* m_channel = nullptr;
};

#endif // RAVENMONACO_H
