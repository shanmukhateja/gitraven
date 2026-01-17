#ifndef RAVENMONACOPAGE_H
#define RAVENMONACOPAGE_H

#include <QWebEnginePage>

#include "gitmanager.h"

class RavenMonacoPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit RavenMonacoPage(QObject *parent = nullptr);

    void init();
    bool isInitFinished() {return m_initFinished;}
    void setReadonly(bool readonly);
    void setTheme(Qt::ColorScheme colorScheme);
    void updateText(GitManager::GitDiffItem diffItem);
signals:
    void signalInitFinished();
private:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                          const QString &message, int lineNumber,
                                          const QString &sourceID) override;

    bool m_initFinished = false;
};

#endif // RAVENMONACOPAGE_H
