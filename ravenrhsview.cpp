#include "ravenrhsview.h"
#include "raveneditor.h"

#include <QTimer>

RavenRHSView::RavenRHSView(RavenStatusMessageDispatcher *statusMsgDispatcher, QWidget *parent)
    : QWidget{parent}, m_mainWindow(static_cast<MainWindow *>(topLevelWidget()->window())),
      m_ravenEditor{new RavenEditor(statusMsgDispatcher, this)}, m_landingInfoWidget(new QWidget(this)),
      m_stackedWidget(new QStackedWidget(this)) {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_stackedWidget);

    // Page 0: Landing Info
    m_landingInfoWidget = new QWidget(this);
    initLandingInfo();
    m_stackedWidget->addWidget(m_landingInfoWidget);

    // Page 1: The Editor (Create it ONCE)
    m_ravenEditor = new RavenEditor(statusMsgDispatcher, this);
    m_stackedWidget->addWidget(m_ravenEditor);

    // Start with Landing Info
    m_stackedWidget->setCurrentWidget(m_landingInfoWidget);

    // Event listeners
    m_ravenTree = m_mainWindow->getRavenLHSView()->getRavenTree();
    connect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::renderDiffItem);
}

RavenRHSView::~RavenRHSView() {
    // cleanup
    disconnect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::renderDiffItem);
}

void RavenRHSView::renderDiffItem(GitManager::GitDiffItem item) {
    qDebug() << "RavenRHSView::renderDiffItem called";

    m_stackedWidget->setCurrentWidget(m_ravenEditor);
    m_ravenEditor->openDiffItem(item);
}

void RavenRHSView::initLandingInfo() {
    // Add landing info widget to `this->layout()`
    auto widget = m_landingInfoWidget;
    layout()->addWidget(widget);

    // Create a layout for landing info widget
    // so we can add child widgets (icon, text)

    auto layout = new QVBoxLayout(widget);
    layout->setAlignment(Qt::AlignCenter);

    auto icon = QIcon::fromTheme("git");
    auto iconLabel = new QLabel(widget);
    iconLabel->setPixmap(icon.pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignHCenter);

    auto *label = new QLabel("GitRaven", widget);
    label->setAlignment(Qt::AlignHCenter);
    label->setStyleSheet("QLabel {font-size: 20px;}");

    layout->addWidget(iconLabel);
    layout->addWidget(label);

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
