#ifndef RAVENSTATUSMESSAGEDISPATCHER_H
#define RAVENSTATUSMESSAGEDISPATCHER_H

#include <QObject>

class RavenStatusMessageDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit RavenStatusMessageDispatcher(QObject *parent = nullptr);

signals:
    void showMessage(QString msg);
};

#endif // RAVENSTATUSMESSAGEDISPATCHER_H
