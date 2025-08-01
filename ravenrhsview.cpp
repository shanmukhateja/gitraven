#include "ravenrhsview.h"

#include "raveneditor.h"

#include <QLabel>

RavenRHSView::RavenRHSView(QWidget *parent)
    : QWidget{parent},
    m_mainWindow(static_cast<MainWindow*>(topLevelWidget()->window())),
    m_ravenEditor(new RavenEditor(this)),
    m_landingInfoWidget(new QWidget(this))
{
    // Widget config
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(m_landingInfoWidget);

    updateUI(std::nullopt);

    m_ravenTree = m_mainWindow->getRavenLHSView()->getRavenTree();
    connect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::updateUI);
}

RavenRHSView::~RavenRHSView()
{
    // cleanup
    disconnect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::updateUI);
}

void RavenRHSView::updateUI(std::optional<GitManager::GitDiffItem> item)
{
    qDebug() << "RavenRHSView::updateUI called";

    m_showLandingInfo = !item.has_value();

    if (!m_showLandingInfo) {
        m_landingInfoWidget->hide();
        layout()->addWidget(m_ravenEditor);
        m_ravenEditor->openDiffItem(std::move(item.value()));
    } else {
        initLandingInfo();
    }
}

void RavenRHSView::initLandingInfo()
{
    auto widget = m_landingInfoWidget;

    auto layout = new QGridLayout(widget);
    layout->setAlignment(Qt::AlignCenter);
    widget->setLayout(layout);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *label = new QLabel(widget);
    label->setText("GitRaven");
    auto icon = QIcon::fromTheme("git");
    auto iconLabel = new QLabel(widget);
    iconLabel->setPixmap(icon.pixmap(64, 64));

    layout->addWidget(iconLabel);
    layout->addWidget(label);
}
