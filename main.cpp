#include "gitmanager.h"
#include "mainwindow.h"
#include "raventree.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

void init(QString gitRepoPath, MainWindow &w) {
    // Git init
    GitManager *manager = new GitManager(gitRepoPath);
    int result = manager->init();
    if (result != 0) {
        QString msgStr = QString("Failed to load Git repository at '%1', error: %2").arg(gitRepoPath).arg(QString::number(result));
        QMessageBox msg(QMessageBox::Critical, "GitRaven", msgStr, QMessageBox::Ok, w.topLevelWidget());
        msg.exec();
        std::exit(0);
        return;
    }

    // Show window
    w.setGitManager(manager);
    w.show();

    // Attach listener for git status change events here
    auto ravenTree = w.getRavenLHSView()->getRavenTree();
    QObject::connect(manager, &GitManager::statusChanged, ravenTree, &RavenTree::buildTree);
    // Change status bar's branch name whenever Git status is called
    // Note: Is this the right place for this code?
    QObject::connect(manager, &GitManager::statusChanged, &w, [&w](GitManager::status_data sd) {
        emit w.statusBar()->signalHEADChange(sd.headStatus);
    });
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationDomain("com.github.shanmukhateja");
    a.setApplicationName("gitraven-qt");
    a.setWindowIcon(QIcon::fromTheme("git"));

    MainWindow w;

    std::optional<QString> gitRepoPath;
    if (argc > 1) {
        gitRepoPath = argv[1];
    } else {
        // Show dialog to select repo directory
        gitRepoPath = QFileDialog::getExistingDirectory(&w,
                                                        "Open Directory",
                                                        nullptr,
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks
                                                        );
    }

    if (gitRepoPath.has_value()) {
        init(gitRepoPath.value(), w);
        return a.exec();
    }

    return 0;
}

