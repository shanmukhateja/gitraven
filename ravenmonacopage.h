#ifndef RAVENMONACOPAGE_H
#define RAVENMONACOPAGE_H

#include <QWebEnginePage>

#include "gitmanager.h"

class RavenMonacoPage : public QWebEnginePage
{
public:
    explicit RavenMonacoPage(QObject *parent = nullptr);
    void setReadonly(bool readonly);
    void updateText(GitManager::GitDiffItem diffItem);
private:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                          const QString &message, int lineNumber,
                                          const QString &sourceID) override;

};

#endif // RAVENMONACOPAGE_H
