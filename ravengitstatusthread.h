#ifndef RAVENGITSTATUSTHREAD_H
#define RAVENGITSTATUSTHREAD_H

#include "gitmanager.h"

#include <QThread>

class RavenGitStatusThread : public QThread
{
    Q_OBJECT
public:
    explicit RavenGitStatusThread(QObject *parent);
protected:
    void run() override;
signals:
    void resultReady(GitManager::status_data payload);
private:
    static int status_cb(const char *path, unsigned int status_flags, void *payload);

    git_status_options m_statusOpts;
    GitManager *m_gitManager;
};

#endif // RAVENGITSTATUSTHREAD_H
