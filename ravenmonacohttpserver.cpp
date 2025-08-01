#include "ravenmonacohttpserver.h"

#include <filesystem>

#include "ravenutils.h"

using std::filesystem::absolute;
using std::filesystem::path;

RavenMonacoHTTPServer::RavenMonacoHTTPServer(QObject *parent)
    : QObject(parent) {}

RavenMonacoHTTPServer::~RavenMonacoHTTPServer()
{
    m_tcpserver->close();
}

int RavenMonacoHTTPServer::init()
{
    qDebug() << "RavenMonacoHTTPServer::init() called";

    if (!m_tcpserver->listen(QHostAddress::LocalHost, PORT) || !m_server->bind(m_tcpserver)) {
        qDebug() << "RavenMonacoHTTPServer::init() Failed to bind port for HTTP server.";
        return -1;
    }

    m_server->route("/<arg>", [] (const QUrl &url) {
        QString urlPath = url.path();

        if (urlPath.length() < 5) {
            return QHttpServerResponse("");
        }

        // Locate editor directory
        path editorDirStdPath = path(RavenUtils::getEditorDirPath());
        path absolutePath = absolute(editorDirStdPath / path(urlPath.toStdString()));

        return QHttpServerResponse::fromFile(QString::fromStdString(absolutePath));
    });

    return 0;
}
