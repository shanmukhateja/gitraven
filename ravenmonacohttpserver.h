#ifndef RAVENMONACOHTTPSERVER_H
#define RAVENMONACOHTTPSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QTcpServer>

class RavenMonacoHTTPServer : QObject
{
    Q_OBJECT
public:
    RavenMonacoHTTPServer(QObject *parent);
    ~RavenMonacoHTTPServer();

    int init();

private:
    QUrl *m_url;
    int PORT = 9191;

    QHttpServer *m_server = new QHttpServer(this);
    QTcpServer *m_tcpserver = new QTcpServer(this);
};

#endif // RAVENMONACOHTTPSERVER_H
