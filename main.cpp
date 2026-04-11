#include "gitmanager.h"
#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    QApplication::setOrganizationDomain("com.github.shanmukhateja");
    QApplication::setApplicationName("gitraven-qt");
    QApplication::setWindowIcon(QIcon::fromTheme("git"));

    std::optional<QString> gitRepoPath;
    if (argc > 1) {
        bool isDir = std::filesystem::is_directory(argv[1]);
        if (isDir)
            gitRepoPath = argv[1];
    }

    if (!gitRepoPath.has_value()) {
        // Show dialog to select repo directory
        auto repoPath = QFileDialog::getExistingDirectory(nullptr, "Open Directory", nullptr,
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!repoPath.isEmpty())
            gitRepoPath = repoPath;
    }

    if (!gitRepoPath.has_value())
        return 0;

    // Git init
    // GitManager will be owned by `app`.
    // This way, MainWindow and it's widgets can acquire it via `qApp`
    auto* manager = new GitManager(gitRepoPath.value(), &app);

    // MainWindow
    MainWindow w;
    w.show();

    return QApplication::exec();
}
