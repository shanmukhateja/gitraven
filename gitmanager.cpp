#include "gitmanager.h"
#include "ravengitstatusthread.h"
#include <QFile>
#include <QMessageBox>
#include <qdebug.h>

namespace fs = std::filesystem;

GitManager::GitManager(QString dir, QObject *parent)
    : QObject{parent},
    m_repoPath{dir}
{
    Q_ASSERT(!m_repoPath.isEmpty());

    // Init Git library
    git_libgit2_init();
    int result = init();
    if (result != 0) {
        QString msgStr = QString("Failed to load Git repository at '%1', error: %2").arg(m_repoPath).arg(QString::number(result));
        QMessageBox msg(QMessageBox::Critical, "GitRaven", msgStr, QMessageBox::Ok, (QWidget*)parent);
        msg.exec();
        std::exit(0);
    }
}

GitManager::~GitManager()
{
    // cleanup
    git_repository_free(m_repo);
    git_libgit2_shutdown();
}

int GitManager::init()
{
    // Prepare dir path
    auto path = m_repoPath.toStdString();

    // Open repository
    return git_repository_open(&m_repo, path.c_str());
}

QList<GitManager::GitBranchSelectorItem> GitManager::getAllBranchesAndTags()
{
    qDebug() << "GitManager::getAllBranchesAndTags called";
    QList<GitManager::GitBranchSelectorItem> results;

    // Fetch all branches
    git_branch_iterator *iter = nullptr;
    git_branch_t branchType{ GIT_BRANCH_ALL };
    git_branch_iterator_new(&iter, m_repo, branchType);
    git_reference *out;
    while (git_branch_next(&out, &branchType, iter) == 0)
    {
        // Get name
        const char *bName = "";
        git_branch_name(&bName, out);
        QString branchName = QString::fromStdString(bName);
        if (!branchName.isEmpty())
        {
            auto item = GitManager::GitBranchSelectorItem {GIT_HEAD_TYPE_BRANCH, branchName, branchName.startsWith("origin")};
            results.append(item);
        }
    }

    // Fetch all tags
    git_strarray arr;
    git_tag_list(&arr, m_repo);
    for (int i=0;i<arr.count;i++)
    {
        // FIXME: Investigate `isRemote` value here.
        auto item = GitManager::GitBranchSelectorItem {GIT_HEAD_TYPE_TAG, arr.strings[i], false};
        results.append(item);
    }

    git_strarray_free(&arr);

    return results;
}

void GitManager::statusAsync()
{
    qDebug() << "GitManager::statusAsync() called";
    RavenGitStatusThread *workerThread = new RavenGitStatusThread(this);
    connect(workerThread, &RavenGitStatusThread::resultReady, this, &GitManager::statusChanged);
    connect(workerThread, &RavenGitStatusThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

GitManager::GitHEADStatus GitManager::findHEADStatus()
{
    GitHEADStatus result = {};

    git_reference *ref;
    git_repository_head(&ref, m_repo);

    // Find branch for HEAD
    git_object *obj;

    if (git_reference_is_branch(ref) == 1)
    {
        // [BRANCH] EASY WAY
        const char *branchName = "";
        git_branch_name(&branchName, ref);
        result.name = branchName;
        result.type = GIT_HEAD_TYPE_BRANCH;
    }
    else if (git_reference_peel(&obj, ref, GIT_OBJECT_COMMIT) == 0)
    {
        // [COMMIT] Set commit hash as branchName
        result.name = oid_to_str(*git_object_id(obj)).toUtf8();
        result.type = GIT_HEAD_TYPE_COMMIT;
        git_object_free(obj);
    }
    else if (git_reference_peel(&obj, ref, GIT_OBJECT_TAG) == 0)
    {
        // [TAG] Locate commit by tag name
        // FIXME:   Show tag name instead of commit hash.
        git_tag *tag;
        int error = git_tag_lookup(&tag, m_repo, git_object_id(obj));
        if (error == 0)
        {
            const char *tagName = git_tag_name(tag);
            result.name = QString::fromStdString(tagName);
            result.type = GIT_HEAD_TYPE_TAG;
        }

        git_tag_free(tag);
    }

    // cleanup
    git_reference_free(ref);

    return result;
}

GitManager::GitStageResponseCode GitManager::stageItem(RavenTreeItem *item)
{
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    if (error != 0)
    {
        qWarning() << "Failed to locate index in the repository.";
        return GitStageResponseCode::INDEX_NOT_FOUND;
    }

    auto pathStdString = item->fullPath.toStdString();
    auto pathCharArray = pathStdString.data();
    git_strarray pathspec = {.count=1};
    pathspec.strings = {&pathCharArray};

    qDebug() << "[GitManager] staging item=" << pathStdString;

    error = git_index_add_all(index, &pathspec, GIT_INDEX_ADD_CHECK_PATHSPEC, NULL, NULL);
    if (error != 0)
    {
        qWarning() << "Failed to index given file" << item->fullPath;
        return GitStageResponseCode::FAILED_TO_INDEX;
    }

    // Save the index changes to disk
    error = git_index_write(index);
    if (error != 0)
    {
        qWarning() << "Failed to write index changes to disk" << error;
        return GitStageResponseCode::FAILED_WRITE_INDEX_TO_DISK;
    }

    git_index_free(index);

    // All is well
    return GitStageResponseCode::DONE;
}

GitManager::GitStageResponseCode GitManager::unstageItem(RavenTreeItem *item)
{
    git_index *index = nullptr;
    auto pathStdString = item->fullPath.toStdString();

    int error = git_repository_index(&index, m_repo);

    if (error != 0)
    {
        qWarning() << "Failed to locate index in the repository.";
        return GitStageResponseCode::INDEX_NOT_FOUND;
    }

    // Generate pathspec from treeItem
    auto pathCharArray = pathStdString.data();
    git_strarray pathspec = {.count=1};
    pathspec.strings = {&pathCharArray};

    // Find HEAD commit & reset the given file to it
    git_reference *head;
    git_object *head_commit;
    git_repository_head(&head, m_repo);
    git_reference_peel(&head_commit, head, GIT_OBJ_COMMIT);

    error = git_reset_default(m_repo, head_commit, &pathspec);

    if (error != 0)
    {
        qWarning() << "Failed to remove given file from index." << item->fullPath;
        return GitStageResponseCode::FAILED_TO_UNSTAGE;
    }

    // Save the index changes to disk
    error = git_index_write(index);
    if (error != 0)
    {
        qWarning() << "Failed to write index changes to disk" << error;
        return GitStageResponseCode::FAILED_WRITE_INDEX_TO_DISK;
    }

    git_index_free(index);

    // All is well
    return GitStageResponseCode::DONE;
}

GitManager::GitDiffItem GitManager::diff(RavenTreeItem *item)
{
    qDebug() << "GitManager::diff called";

    auto path = item->fullPath;
    auto absPath = item->absolutePath;
    auto status = (int)item->flag;
    auto initiator = item->initiator;

    auto repo = m_repo;

    GitDiffItem diffItem = {};

    if (item->binary) {
        qWarning() << "Unsupported file type, expected text-like, found binary";
        return diffItem;
    }

    // New file has been added to index/working directory & it's first time we're seeing it.
    if (status == GIT_STATUS_INDEX_NEW || status == GIT_STATUS_WT_NEW)
    {
        qDebug() << "(status == GIT_STATUS_INDEX_NEW || status == GIT_STATUS_WT_NEW)";

        auto fileContent = getLocalFileContent(absPath);
        if (fileContent.has_value())
        {
            diffItem.newFileContent = fileContent->content;
            diffItem.newFilePath = absPath;
        }

        return diffItem;
    }

    // File exists in index previously and it has been modified and add to staging area.
    else if (status == GIT_STATUS_INDEX_MODIFIED)
    {
        qDebug() << "(status == GIT_STATUS_INDEX_MODIFIED)";

        git_object *obj = NULL;
        int error = git_revparse_single(&obj, repo, "HEAD^{tree}");

        git_tree *tree = NULL;
        error = git_tree_lookup(&tree, repo, git_object_id(obj));

        git_diff *diff = NULL;
        error = git_diff_tree_to_index(&diff, repo, tree, NULL, NULL);

        diff_data payload = {.reqFilePath = path};
        git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

        auto oldfileContent = getFileContent(payload.old_oid);
        auto newfileContent = getFileContent(payload.new_oid);

        // qDebug() << "old="<< oldfileContent.has_value() << "new=" << newfileContent.has_value();

        if (oldfileContent.has_value())
        {
            diffItem.oldFileContent = oldfileContent->content;
            diffItem.oldFilePath = payload.oldAbsPath;
        }
        if (newfileContent.has_value())
        {
            diffItem.newFileContent = newfileContent->content;
            diffItem.newFilePath = payload.newAbsPath;
        }

        git_tree_free(tree);
        git_object_free(obj);
        git_diff_free(diff);

        return diffItem;
    }

    // An existing file is modified but not staged.
    else if (status == GIT_STATUS_WT_MODIFIED)
    {
        // Show changes from HEAD to working directory

        qDebug() << "(GIT_STATUS_WT_MODIFIED)";

        git_object *obj = NULL;
        int error = git_revparse_single(&obj, repo, "HEAD^{tree}");

        git_tree *tree = NULL;
        error = git_tree_lookup(&tree, repo, git_object_id(obj));

        git_diff *diff = NULL;
        error = git_diff_tree_to_workdir_with_index(&diff, repo, tree, NULL);

        diff_data payload = {.reqFilePath = path, .repoPath = m_repoPath};
        git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

        auto oldfileContent = getFileContent(payload.old_oid);
        auto newfileContent = getFileContent(payload.new_oid);

        // When file is modified on disk but doesn't have blob id,
        // we read from disk instead.
        // Note: VS Code doesn't seem to call `getFileContent`
        //       at all, it uses getLocalFileContent acc. to Git
        //       output list.
        if (!newfileContent.has_value())
        {
            qDebug() << "Failed to get file content from git, using filesystem:"<<absPath;
            // Unable to locate file contents from git, using filesytem as source
            newfileContent = getLocalFileContent(absPath);
            diffItem.newFilePath = absPath;
        }

        if (oldfileContent.has_value())
        {
            diffItem.oldFileContent = oldfileContent->content;
            diffItem.oldFilePath = payload.oldAbsPath;
        }
        if (newfileContent.has_value())
        {
            diffItem.newFileContent = newfileContent->content;
            diffItem.newFilePath = payload.newAbsPath;
        }

        git_tree_free(tree);
        git_object_free(obj);
        git_diff_free(diff);
    }

    // An existing file has been be renamed or deleted.
    // TODO: We need to detect this and show the change in UI
    else if (status == (GIT_STATUS_WT_DELETED))
    {
        qDebug() << "(GIT_STATUS_WT_DELETED)";

        // Show file contents from index.
        // Note: We do not have newFileContent as the file has been deleted.

        git_diff *diff = NULL;
        int error = git_diff_index_to_workdir(&diff, repo, NULL, NULL);

        diff_data payload = {.reqFilePath = path};
        git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

        auto oldfileContent = getFileContent(payload.old_oid);

        if (oldfileContent.has_value())
        {
            diffItem.oldFileContent = oldfileContent->content;
            diffItem.oldFilePath = payload.oldAbsPath;
        }

        git_diff_free(diff);
    }

    // Could be a new file that was staged and it contains some uncommitted changes.
    else if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_MODIFIED))
    {
        qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_MODIFIED)";

        // We need to show changes from index to working directory

        if (initiator == RavenTreeItem::STAGING)
        {
            // We need to pull HEAD to staging area changes
            git_object *obj = NULL;
            int error = git_revparse_single(&obj, repo, "HEAD^{tree}");

            git_tree *tree = NULL;
            error = git_tree_lookup(&tree, repo, git_object_id(obj));

            git_diff *diff = NULL;
            error = git_diff_tree_to_index(&diff, repo, tree, NULL, NULL);

            diff_data payload = {.reqFilePath = path};
            git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

            auto oldfileContent = getFileContent(payload.old_oid);
            auto newfileContent = getFileContent(payload.new_oid);

            // qDebug() << "old="<< oldfileContent.has_value() << "new=" << newfileContent.has_value();

            if (oldfileContent.has_value())
            {
                diffItem.oldFileContent = oldfileContent->content;
                diffItem.oldFilePath = payload.oldAbsPath;
            }
            if (newfileContent.has_value())
            {
                diffItem.newFileContent = newfileContent->content;
                diffItem.newFilePath = payload.newAbsPath;
            }

            git_tree_free(tree);
            git_object_free(obj);
            git_diff_free(diff);

            return diffItem;
        }
        else if (initiator == RavenTreeItem::UNCOMMITTED)
        {
            // Need to pull from staging area and working directory

            git_diff *diff = NULL;
            int error = git_diff_index_to_workdir(&diff, repo, NULL, {});

            qDebug() << "error=" << error;

            diff_data payload = {.reqFilePath = path};
            git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

            auto oldfileContent = getFileContent(payload.old_oid);
            auto newfileContent = getLocalFileContent(absPath);

            //qDebug() << "old OID="<< oid_to_str(payload.old_oid) << "new OID=" << oid_to_str(payload.new_oid);

            if (oldfileContent.has_value())
            {
                diffItem.oldFileContent = oldfileContent->content;
                diffItem.oldFilePath = payload.oldAbsPath;
            }
            if (newfileContent.has_value())
            {
                diffItem.newFileContent = newfileContent->content;
                diffItem.newFilePath = payload.newAbsPath;
            }

            git_diff_free(diff);

            return diffItem;
        }
    }

    // Could be a new file that was staged but it is now deleted in working directory
    else if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_DELETED))
    {
        qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_DELETED)";

        if (initiator == RavenTreeItem::STAGING)
        {
            // We need to pull HEAD to staging area changes
            git_object *obj = NULL;
            int error = git_revparse_single(&obj, repo, "HEAD^{tree}");

            git_tree *tree = NULL;
            error = git_tree_lookup(&tree, repo, git_object_id(obj));

            git_diff *diff = NULL;
            error = git_diff_tree_to_index(&diff, repo, tree, NULL, NULL);

            diff_data payload = {.reqFilePath = path};
            git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

            // Note: We do not compute oldFileContent because the file is new
            //       to us and there are no other changes of that file.
            auto newfileContent = getFileContent(payload.new_oid);

            if (newfileContent.has_value())
            {
                diffItem.newFileContent = newfileContent->content;
                diffItem.newFilePath = payload.newAbsPath;
            }

            git_tree_free(tree);
            git_object_free(obj);
            git_diff_free(diff);

            return diffItem;
        }
        else if (initiator == RavenTreeItem::UNCOMMITTED)
        {
            // TODO: track file deleted status and return the value
            //       For now, we send empty diffItem
            return diffItem;
        }
    }

    // Could be a new file that was staged but it is renamed in working directory
    // Note: This might mean the repo is corrupted.
    else if (status == (GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_RENAMED))
    {
        qDebug() << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_RENAMED)";

        qWarning() << "The scenario" << "(GIT_STATUS_INDEX_NEW|GIT_STATUS_WT_RENAMED)" << "is not supported by GitRaven.";
        return diffItem;
    }

    // Could be a file already staged and modified but it contains some uncommitted changes
    else if (status == (GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_MODIFIED))
    {
        qDebug() << "(GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_MODIFIED)";

        if (initiator == RavenTreeItem::STAGING)
        {
            // We need to pull HEAD to staging area changes
            git_object *obj = NULL;
            int error = git_revparse_single(&obj, repo, "HEAD^{tree}");

            git_tree *tree = NULL;
            error = git_tree_lookup(&tree, repo, git_object_id(obj));

            git_diff *diff = NULL;
            error = git_diff_tree_to_index(&diff, repo, tree, NULL, NULL);

            diff_data payload = {.reqFilePath = path};
            git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

            auto oldfileContent = getFileContent(payload.old_oid);
            auto newfileContent = getFileContent(payload.new_oid);

            // qDebug() << "old="<< oldfileContent.has_value() << "new=" << newfileContent.has_value();

            if (oldfileContent.has_value())
            {
                diffItem.oldFileContent = oldfileContent->content;
                diffItem.oldFilePath = payload.oldAbsPath;
            }
            if (newfileContent.has_value())
            {
                diffItem.newFileContent = newfileContent->content;
                diffItem.newFilePath = payload.newAbsPath;
            }

            git_tree_free(tree);
            git_object_free(obj);
            git_diff_free(diff);

            return diffItem;
        }
        else if (initiator == RavenTreeItem::UNCOMMITTED)
        {
            // Need to pull from staging area and working directory

            git_diff *diff = NULL;
            int error = git_diff_index_to_workdir(&diff, repo, NULL, {});

            qDebug() << "error=" << error;

            diff_data payload = {.reqFilePath = path};
            git_diff_foreach(diff, each_file_cb, each_binary_file_cb, NULL, NULL, &payload);

            auto oldfileContent = getFileContent(payload.old_oid);
            auto newfileContent = getLocalFileContent(absPath);

            //qDebug() << "old OID="<< oid_to_str(payload.old_oid) << "new OID=" << oid_to_str(payload.new_oid);

            if (oldfileContent.has_value())
            {
                diffItem.oldFileContent = oldfileContent->content;
                diffItem.oldFilePath = payload.oldAbsPath;
            }
            if (newfileContent.has_value())
            {
                diffItem.newFileContent = newfileContent->content;
                diffItem.newFilePath = payload.newAbsPath;
            }

            git_diff_free(diff);

            return diffItem;
        }
    }
    // Could be a file already staged and modified but it is now deleted in working directory
    else if (status == (GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_DELETED))
    {
        qDebug() << "(GIT_STATUS_INDEX_MODIFIED|GIT_STATUS_WT_DELETED)";
    }
    else
    {
        qWarning() << "Unsupported status '" << status << "'";
    }

    return diffItem;
}

std::optional<GitManager::RavenFile> GitManager::getFileContent(git_oid oid)
{
    qDebug() << "GitManager::getFileContent called";

    GitManager::RavenFile ravenFile;

    // Find blob for given object id
    git_blob *blob;
    int result = git_blob_lookup(&blob, m_repo, &oid);

    if (result != 0) {
        qWarning() << "Unable to lookup blob for oid, error =" << result;
        return std::nullopt;
    }

    // read file content
    int rawsize = static_cast<int>(git_blob_rawsize(blob));
    const char *rawcontent = static_cast<const char*>(git_blob_rawcontent(blob));

    // return QString
    ravenFile.content = QString::fromUtf8(rawcontent, rawsize);
    ravenFile.binary = git_blob_is_binary(blob);

    return ravenFile;
}

std::optional<GitManager::RavenFile> GitManager::getLocalFileContent(QString absPath)
{
    RavenFile ravenFile;

    QFile file = QFile(absPath);
    if (file.open(QIODevice::ReadOnly))
    {
        ravenFile.content = QString::fromStdString(file.readAll().toStdString());
        return ravenFile;
    }

    return std::nullopt;
}

int GitManager::each_file_cb(const git_diff_delta *delta, float progress, void *payload)
{
    qDebug() << "GitManager::each_file_cb called";

    diff_data *d = (diff_data*)payload;

    auto newFile = delta->new_file;
    auto oldFile = delta->old_file;
    auto status = delta->status;

    // qDebug() << "newfile:" << newFile.path;
    // qDebug() << "oldfile:" << oldFile.path;
    // qDebug() << "status=" << status;

    if (d->reqFilePath != newFile.path && d->reqFilePath != oldFile.path) return 0;

    d->old_oid = oldFile.id;
    d->new_oid = newFile.id;

    d->newPath = newFile.path;
    d->oldPath = oldFile.path;

    std::filesystem::path repoPath(d->repoPath.toStdString());
    d->newAbsPath = QString::fromStdString(std::filesystem::absolute(repoPath / newFile.path));
    d->oldAbsPath = QString::fromStdString(std::filesystem::absolute(repoPath / oldFile.path));

    d->status = status;
    return 0;
}

/**
 * @brief GitManager::checkoutToRef
 * Checkout in `libgit2` doesn't switch branch, it simply checks files out on disk.
 * Hence, we must call `git_repository_set_head` on success.
 * More info: https://stackoverflow.com/a/46758861
 * @param item The payload contains data used to checkout to ref (branch/tag)
 * @return `nullptr` on success or QString with error message.
 */
QString GitManager::checkoutToRef(GitBranchSelectorItem item)
{
    qDebug() << "GitManager::slotCheckoutToRef called";

    std::string refName = this->generateRefName(&item);
    int error = -999;
    git_object *treeish = nullptr;
    error = git_revparse_single(&treeish, m_repo, refName.c_str());
    // qDebug() << "revparse error=" << error;
    if (error != 0) return getCheckoutErrorMessage();
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    error = git_checkout_tree(m_repo, treeish, &opts);
    // qDebug() << "checkout_tree error=" << error;
    if (error != 0)
    {
        git_object_free(treeish);
        return getCheckoutErrorMessage();
    }
    error = git_repository_set_head(m_repo, refName.c_str());

    // FIXME: checkout to remote branches or tags lead to detached HEAD state.

    if (error == 0)
    {
        // All ok
        git_object_free(treeish);
        // Update UI
        // Note: Is this right place to call this function?
        statusAsync();
        return nullptr;
    }
    else
    {
        git_object_free(treeish);
        // Failed to checkout, report error.
        return getCheckoutErrorMessage();
    }
}

std::string GitManager::generateRefName(GitManager::GitBranchSelectorItem *item)
{
    qDebug() << "GitManager::generateRef called with item.name=" << item->name;

    QString result;

    if (item->isRemote)
    {
        result.append("refs/remotes/");
    }
    else if (item->type == GIT_HEAD_TYPE_BRANCH)
    {
        result.append("refs/heads/");
    }
    else if (item->type == GIT_HEAD_TYPE_TAG)
    {
        result.append("refs/tags/");
    }
    else if (item->type == GIT_HEAD_TYPE_COMMIT)
    {
        qCritical() << "GitManager::generateRef expects branch/tag, got commit instead.";
    }

    result.append(item->name);

    qDebug() << "GitManager::generateRef final value=" << result;

    return result.toStdString();
}

QString GitManager::getCheckoutErrorMessage()
{
    auto errorObj = git_error_last();
    return QString::fromStdString(errorObj->message);
}

int GitManager::each_binary_file_cb(const git_diff_delta *delta, const git_diff_binary *binary, void *payload)
{
    qDebug() << "GitManager::each_binary_file_cb called";
    return 0;
}

QString GitManager::oid_to_str(git_oid oid)
{
    char c[GIT_OID_HEXSZ + 1] = {}; // +1 for null terminator
    git_oid_tostr(c, sizeof(c), &oid);
    return QString::fromStdString(c);
}

int GitManager::commit(QList<QString> items, QString msg, bool amend)
{
    qDebug() << "GitManager::commit called";

    git_index *index;
    git_object *parent = NULL;
    git_reference *ref = NULL;
    git_signature *committer;
    git_oid *tree_oid, *commit_oid;
    git_tree *tree;
    // amend
    git_oid *amendCommitOid;
    git_commit *amendCommit;

    int error = git_revparse_ext(&parent, &ref, m_repo, "HEAD");
    if (error != 0)
    {
        qCritical() << "Unable to find HEAD in the repository.";
        return error;
    }

    error = git_repository_index(&index, m_repo);
    if (error != 0)
    {
        qCritical() << "Unable to locate index for given repository";
        return error;
    }

    git_strarray pathspec = {};
    auto size = items.size();
    pathspec.strings = new char*[size + 1];
    pathspec.count = items.count();
    for (int i = 0; i < size; ++i) {
        pathspec.strings[i] = strdup(items[i].toStdString().c_str());
    }

    error = git_index_add_all(index, &pathspec, GIT_INDEX_ADD_CHECK_PATHSPEC, NULL, NULL);
    if (error != 0)
    {
        qCritical() << "Unable to add items to index.";
        return error;
    }

    error = git_index_write_tree(tree_oid, index);
    if (error != 0)
    {
        qCritical() << "Unable to write tree to disk.";
        return error;
    }

    error = git_index_write(index);
    if (error != 0)
    {
        qCritical() << "Unable to write index to disk.";
        return error;
    }

    error = git_tree_lookup(&tree, m_repo, tree_oid);
    if (error != 0)
    {
        qCritical() << "Unable to find tree for given oid in the repository.";
        return error;
    }

    error = git_signature_default(&committer, m_repo);
    if (error != 0)
    {
        qCritical() << "Unable to find user details from Git. Is Git configured?";
        return error;
    }

    if (!amend)
    {
        // create commit
        error = git_commit_create_v(
            commit_oid,
            m_repo,
            "HEAD",
            committer,
            committer,
            NULL,
            msg.toStdString().c_str(),
            tree,
            parent ? 1: 0,
            parent
        );
    }
    else
    {
        // amend commit

        error = git_reference_name_to_id(amendCommitOid, m_repo, "HEAD");
        if (error != 0)
        {
            qCritical() << "Unable to locate reference to HEAD commit from repository.";
            return error;
        }

        // FIXME: Use `amendCommitOid` instead
        // Note:  The app seg faults when this variable is deleted :/
        git_oid *oid = amendCommitOid;
        error = git_commit_lookup(&amendCommit, m_repo, oid);
        if (error != 0)
        {
            qCritical() << "Unable to locate HEAD commit for " << oid_to_str(*oid);
            return error;
        }

        // amend commit
        error = git_commit_amend(
            commit_oid,
            amendCommit,
            "HEAD",
            committer,
            committer,
            NULL,
            msg.toStdString().c_str(),
            tree
        );
    }

    if (error != 0)
    {
        qCritical() << "Unable to commit the staged items, error: "<<error;
        return error;
    }

    git_index_free(index);
    git_signature_free(committer);
    git_tree_free(tree);
    git_object_free(parent);
    git_reference_free(ref);
    if (amend)
    {
        git_commit_free(amendCommit);
    }

    return 0;
}
