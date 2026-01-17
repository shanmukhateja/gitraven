#include "ravenmonacopage.h"

#include <QColor>
#include <QGuiApplication>
#include <QJsonObject>
#include <QMessageBox>
#include <QStyleHints>


RavenMonacoPage::RavenMonacoPage(QObject *parent)
    : QWebEnginePage(parent)
{
    // Update QWebEnginePage theme
    // Note:    This is required to avoid "white flash"
    //          when RavenMonaco is loading this page.
    QStyleHints *hints = QGuiApplication::styleHints();
    bool isLightTheme = hints->colorScheme() == Qt::ColorScheme::Light;
    if (!isLightTheme)
    {
        setBackgroundColor(QColor(30,30,30));
    }
}

void RavenMonacoPage::init()
{
    // Init monaco when the page load is finished.
    connect(this, &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qCritical() << "Failed to load Monaco editor, check Monaco HTTP server.";
            QMessageBox errorMsg(QMessageBox::Critical, "GitRaven" , "Failed to load Diff Viewer components.", QMessageBox::Ok);
            errorMsg.exec();
            std::exit(-1);
        }

        // Call init() function
        this->runJavaScript("init()", 0, [this](const QVariant &) {
            // Update Monaco theme
            setTheme(QGuiApplication::styleHints()->colorScheme());
            // emit Monaco init is completed
            m_initFinished = true;
            emit this->signalInitFinished();
        });
    });

    // Load index.html
    load(QUrl("http://localhost:9191/index.html"));
}

void RavenMonacoPage::setReadonly(bool readonly)
{
    runJavaScript(QString("setReadonly({opt})").replace("{opt}", QVariant(readonly).toString()));
}

void RavenMonacoPage::setTheme(Qt::ColorScheme colorScheme)
{
    QJsonObject obj;
    obj["theme"] = colorScheme == Qt::ColorScheme::Light ? "light" : "dark";
    QJsonDocument jd(obj);
    runJavaScript(QString("setTheme({opt})").replace("{opt}", jd.toJson()));
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
    int lineNumber,
    const QString &sourceID
)
{
    qDebug() << "RavenMonacoPage::javaScriptConsoleMessage";
    qDebug() << level << message << lineNumber << sourceID;
}
