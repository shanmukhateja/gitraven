#include "gitmanager.h"
#include "mainwindow.h"
#include "raventree.h"

#include <QApplication>
#include <QFileDialog>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationDomain("com.github.shanmukhateja");
    app.setApplicationName("gitraven-qt");
    app.setWindowIcon(QIcon::fromTheme("git"));

    std::optional<QString> gitRepoPath;
    if (argc > 1) {
        gitRepoPath = argv[1];
    } else {
        // Show dialog to select repo directory
        gitRepoPath = QFileDialog::getExistingDirectory(nullptr,
                                                        "Open Directory",
                                                        nullptr,
                                                        QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks
                                                        );
    }

    // Clean exit
    if (!gitRepoPath.has_value()) return 0;

    // Git init
    GitManager *manager = new GitManager(gitRepoPath.value());

    // MainWindow
    MainWindow w(manager);
    w.show();

    // Attach listener for git status change events here
    // Note: This code maybe moved elsewhere now that
    // GM lifecycle issue is fixed.
    auto ravenTree = w.getRavenLHSView()->getRavenTree();
    QObject::connect(manager, &GitManager::statusChanged, ravenTree, &RavenTree::buildTree);

    return app.exec();
}

