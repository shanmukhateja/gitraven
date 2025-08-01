#include "ravenmonaco.h"

#include <QStyleHints>
#include <QWebChannel>
#include <qmessagebox.h>

namespace fs = std::filesystem;

RavenMonaco::RavenMonaco(QWidget *parent)
    : QWebEngineView{parent}
{
    // Init page
    m_page = new RavenMonacoPage(this);
    setPage(m_page);

    // Light/dark theme switcher
    QStyleHints *hint = QGuiApplication::styleHints();

    // Init monaco when the page load is finished.
    connect(page(), &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qCritical() << "Failed to load Monaco editor, check Monaco HTTP server.";
            QMessageBox errorMsg(QMessageBox::Critical, "GitRaven" , "Failed to load Diff Viewer components.", QMessageBox::Ok);
            errorMsg.exec();
            std::exit(-1);
        }

        // Call init() function
        page()->runJavaScript("init()", 0, [this](const QVariant &) {
            // Update theme
            setTheme(QGuiApplication::styleHints()->colorScheme());
        });
    });

    // Init HTTP server for monaco-editor
    m_server = new RavenMonacoHTTPServer(this);
    m_server->init();

    setDefaultUrl();

    // Init bridge
    m_bridge = new RavenMonacoBridge(this, (RavenEditor*)parent);
    m_channel = new QWebChannel(this);
    m_page->setWebChannel(m_channel);
    m_channel->registerObject("cppBridge", m_bridge);

    // Light/dark theme switcher
    connect(hint, &QStyleHints::colorSchemeChanged, this, &RavenMonaco::setTheme);
}

RavenMonacoPage *RavenMonaco::page() const
{
    return m_page;
}

void RavenMonaco::setDefaultUrl()
{
    setUrl(QUrl("http://localhost:9191/index.html"));
    load(url());
}

void RavenMonaco::setTheme(Qt::ColorScheme colorScheme)
{
    QJsonObject obj;
    obj["theme"] = colorScheme == Qt::ColorScheme::Light ? "light" : "dark";
    QJsonDocument jd(obj);
    m_page->runJavaScript(QString("setTheme({opt})").replace("{opt}", jd.toJson()));
}


