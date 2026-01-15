#include "ravengitcheckoutdialog.h"

#include <QDialog>

#include <QMessageBox>
#include <QVBoxLayout>

RavenGitCheckoutDialog::RavenGitCheckoutDialog(GitManager *manager, QWidget* parent)
    : QDialog{parent},
    m_gitManager{manager},
    m_checkoutStatusLabel{new QLabel(this)},
    m_list{new QListWidget(this)},
    m_searchList{new QLineEdit(this)},
    m_checkoutButton(new QPushButton("Checkout", this))
{
    setWindowTitle("Choose Branch/Tag to checkout");
    resize(600, 500);

    // Create new layout with dialog as parent to "own" the layout.
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_checkoutStatusLabel);
    mainLayout->addWidget(m_searchList);
    mainLayout->addWidget(m_list);
    mainLayout->addWidget(m_checkoutButton, 0, Qt::AlignRight);

    // Setup UI
    m_checkoutStatusLabel->setHidden(true);
    m_checkoutStatusLabel->setWordWrap(true);
    m_searchList->setPlaceholderText("Filter (Type something...)");

    // m_checkoutButton click event
    connect(m_checkoutButton, &QPushButton::clicked, this, [this] {
        auto selectionList = m_list->selectedItems();
        if (selectionList.empty()) return;

        auto selectedItem = selectionList.first()->data(Qt::UserRole).value<GitManager::GitBranchSelectorItem>();

        QString resultOrErrorMsg = m_gitManager->checkoutToRef(selectedItem);
        m_checkoutStatusLabel->setHidden(false);
        if (resultOrErrorMsg != nullptr)
        {
            // Checkout failed, inform user.
            QString msg = QString("Checkout error: '%1'").arg(resultOrErrorMsg);
            m_checkoutStatusLabel->setText(msg);
            m_checkoutStatusLabel->setStyleSheet("background-color:red;color:white;padding:5px");
        }
        else
        {
            // OK, reset warning and hide dialog
            this->resetCheckoutWarningLabel();
            close();
        }
    });

    // Populate list with branches and tags when checkout button is clicked in statusBar.
    connect(
        this,
        &RavenGitCheckoutDialog::signalOnBranchChangeRequested,
        this,
        &RavenGitCheckoutDialog::slotOnBranchChangeRequested
    );

    connect(
        m_searchList,
        &QLineEdit::textChanged,
        this,
        [this](const QString &text)
        {
            for (int i = 0; i < m_list->count(); i++)
            {
                QListWidgetItem *item = m_list->item(i);
                bool match = item->text().contains(text, Qt::CaseInsensitive);
                item->setHidden(!match);
            }
        }
    );

    // When user double-clicks on list item, auto click on Checkout button.
    connect(m_list, &QListWidget::itemDoubleClicked, this, [this] {
        m_checkoutButton->click();
    });
}

void RavenGitCheckoutDialog::slotOnBranchChangeRequested()
{
    qDebug() << "RavenGitCheckoutDialog::slotOnBranchChangeRequested() called";

    // Fetch all branches/tags
    // FIXME: NOT in GUI thread!!
    QList<GitManager::GitBranchSelectorItem> results = m_gitManager->getAllBranchesAndTags();

    // LIST WIDGET
    for (auto item : results)
    {
        QIcon icon;

        // Set icon
        if (item.type == GitManager::GIT_HEAD_TYPE_BRANCH)
        {
            // BRANCH
            if (!item.isRemote)
            {
                icon = QIcon::fromTheme("branch-symbolic");
            }
            else
            {
                icon = QIcon::fromTheme("vcs-branch-symbolic");
            }
        }
        else if (item.type == GitManager::GIT_HEAD_TYPE_TAG)
        {
            // TAG
            icon = QIcon::fromTheme("tag-symbolic");
        }
        else
        {
            Q_UNREACHABLE();
        }

        auto widgetItem = new QListWidgetItem(icon, item.name, m_list, QListWidgetItem::ItemType::UserType);
        widgetItem->setData(Qt::UserRole, QVariant::fromValue(item));
    }

    exec();
}

void RavenGitCheckoutDialog::resetCheckoutWarningLabel()
{
    m_checkoutStatusLabel->setText("");
    m_checkoutStatusLabel->setStyleSheet("");
    m_checkoutStatusLabel->setHidden(true);
}

