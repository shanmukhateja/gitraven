#include "ravenmonacopage.h"

#include <QColor>
#include <QGuiApplication>
#include <QJsonObject>
#include <QStyleHints>


RavenMonacoPage::RavenMonacoPage(QObject *parent)
    : QWebEnginePage(parent)
{
    // Update QWebEnginePage theme
    // Note: This is required to avoid "white flash" when RavenMonaco is loading this page.
    QStyleHints *hints = QGuiApplication::styleHints();
    bool isLightTheme = hints->colorScheme() == Qt::ColorScheme::Light;
    if (!isLightTheme)
    {
        setBackgroundColor(QColor(30,30,30));
    }
}

void RavenMonacoPage::setReadonly(bool readonly)
{
    runJavaScript(QString("setReadonly({opt})").replace("{opt}", QVariant(readonly).toString()));
}

void RavenMonacoPage::updateText(GitManager::GitDiffItem diffItem)
{
    // Build JSON payload
    QJsonObject payloadJ;
    payloadJ["oldText"] = diffItem.oldFileContent;
    payloadJ["oldPath"] = diffItem.oldFilePath;
    payloadJ["newText"] = diffItem.newFileContent;
    payloadJ["newPath"] = diffItem.newFilePath;

    QJsonDocument payloadJD(payloadJ);

    QString payloadJDStr = QString(payloadJD.toJson());

    // Send request
    runJavaScript(QString("update({opt})").replace("{opt}", payloadJDStr));
}

void RavenMonacoPage::javaScriptConsoleMessage(
    JavaScriptConsoleMessageLevel level,
    const QString &message,
    int lineNumber, const
    QString &sourceID
)
{
    qDebug() << "RavenMonacoPage::javaScriptConsoleMessage";
    qDebug() << level << message << lineNumber << sourceID;
}
