#ifndef RAVENGITBRANCHSELECTOR_H
#define RAVENGITBRANCHSELECTOR_H

#include "gitmanager.h"

#include <QWidget>

class RavenGitBranchSelector : public QWidget
{
    Q_OBJECT
public:
    RavenGitBranchSelector(GitManager *manager, QWidget *parent = nullptr);
signals:
    void signalOnBranchChangeRequested();

public slots:
    void slotOnBranchChangeRequested();

private:
    GitManager *m_gitManager;
};

#endif // RAVENGITBRANCHSELECTOR_H
