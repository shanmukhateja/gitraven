#include "ravenmonaco.h"

#include <QGuiApplication>
#include <QStyleHints>
#include <QWebChannel>
#include <qmessagebox.h>

namespace fs = std::filesystem;

RavenMonaco::RavenMonaco(QWidget *parent)
    : QWebEngineView{parent},
    m_page{new RavenMonacoPage(this)}
{
    // Init page
    setPage(m_page);

    // Init HTTP server for monaco-editor
    m_server = new RavenMonacoHTTPServer(this);
    m_server->init();

    // Init bridge
    m_bridge = new RavenMonacoBridge(this, (RavenEditor*)parent);
    m_channel = new QWebChannel(this);
    m_page->setWebChannel(m_channel);
    m_channel->registerObject("cppBridge", m_bridge);

    m_page->init();

    // Light/dark theme switcher
    // FIXME: Can we move this to page instead?
    QStyleHints *hint = QGuiApplication::styleHints();
    connect(hint, &QStyleHints::colorSchemeChanged, page(), &RavenMonacoPage::setTheme);
}

