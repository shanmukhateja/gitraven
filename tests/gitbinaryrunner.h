//
// Created by suryateja on 26/04/26.
//

#ifndef GITRAVEN_GITBINARYRUNNER_H
#define GITRAVEN_GITBINARYRUNNER_H

#include <QProcess>
#include <qdir.h>
#include <qfile.h>

class GitBinaryRunner : public QObject {
    Q_OBJECT
  public:
    explicit GitBinaryRunner(QString repoPath, QObject *parent);

    ~GitBinaryRunner() override;

    [[nodiscard]] bool initGitRepo();
    [[nodiscard]] bool resetGitRepo();
    [[nodiscard]] bool teardownGitRepo() const;

    static bool createFilesForPath(const QString &path, const QString &content);
    static bool updateFileContent(const QString &path, const QString &content);

    bool runGitCommand(const QStringList &arguments);
    QString runGitCommandForOutput(const QStringList &arguments);

  private:
    QString GIT_BINARY_PATH = "/usr/bin/git";
    QString GIT_REPO_PATH = nullptr;
};

#endif // GITRAVEN_GITBINARYRUNNER_H
