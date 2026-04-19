#ifndef RAVENMONACOBRIDGE_H
#define RAVENMONACOBRIDGE_H

#include <QObject>
#include <QDebug>

// Forward declarations
class RavenEditor;

class RavenMonacoBridge : public QObject
{
    Q_OBJECT
public:
    explicit RavenMonacoBridge(QObject *parent = nullptr, RavenEditor *editor = nullptr);
    Q_INVOKABLE void saveModifiedChanges(QString modified);

private:
    RavenEditor *m_ravenEditor;
};

#endif // RAVENMONACOBRIDGE_H
