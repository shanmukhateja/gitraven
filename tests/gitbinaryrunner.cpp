//
// Created by suryateja on 26/04/26.
//

#include "gitbinaryrunner.h"
#include <QDebug>

#include <QDir>
#include <utility>

GitBinaryRunner::GitBinaryRunner(QString repoPath, QObject *parent = nullptr)
    : QObject(parent), GIT_REPO_PATH(std::move(repoPath)) {}

GitBinaryRunner::~GitBinaryRunner() {
    GIT_REPO_PATH = nullptr;
    GIT_BINARY_PATH = nullptr;
}

bool GitBinaryRunner::initGitRepo() {
    const QDir repoDir(GIT_REPO_PATH);
    if (!repoDir.exists()) {
        if (!repoDir.mkpath(".")) {
            qWarning() << "Failed to create directory" << GIT_REPO_PATH;
            return false;
        }
    }

    return runGitCommand({"init"});
}

bool GitBinaryRunner::resetGitRepo() {
    QDir dir(GIT_REPO_PATH);
    if (!dir.exists())
        return true;
    return dir.removeRecursively() && initGitRepo();
}

bool GitBinaryRunner::teardownGitRepo() const {
    QDir repoPath(GIT_REPO_PATH);
    return repoPath.removeRecursively();
}

bool GitBinaryRunner::createFilesForPath(const QString &path, const QString &content) {
    QFileInfo fileInfo(path);

    // Create parent directories if they don't exist
    QDir dir(fileInfo.absolutePath());
    if (!dir.exists()) {
        if (!dir.mkpath("")) {
            return false;
        }
    }

    // Now update the file content (create or overwrite the file)
    return updateFileContent(path, content);
}

bool GitBinaryRunner::updateFileContent(const QString &path, const QString &content) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    QFileInfo fileInfo(path);
    QTextStream out(&file);
    out << content;
    file.flush();
    file.close();

    return true;
}

bool GitBinaryRunner::runGitCommand(const QStringList &arguments = {}) {
    QProcess gitBinary(this);
    gitBinary.setWorkingDirectory(GIT_REPO_PATH);
    gitBinary.start(GIT_BINARY_PATH, arguments);
    return gitBinary.waitForFinished(4000);
}

QString GitBinaryRunner::runGitCommandForOutput(const QStringList &arguments = {}) {
    QProcess gitBinary(this);
    gitBinary.setWorkingDirectory(GIT_REPO_PATH);
    gitBinary.start(GIT_BINARY_PATH, arguments);
    gitBinary.setReadChannel(QProcess::StandardOutput);
    gitBinary.waitForFinished(4000);
    QString out = gitBinary.readAllStandardOutput();
    return out;
}
