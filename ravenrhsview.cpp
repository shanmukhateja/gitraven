#include "ravenrhsview.h"
#include "raveneditor.h"

#include <QTimer>

RavenRHSView::RavenRHSView(RavenStatusMessageDispatcher *statusMsgDispatcher, QWidget *parent)
    : QWidget{parent},
    m_mainWindow(static_cast<MainWindow*>(topLevelWidget()->window())),
    m_ravenEditor{new RavenEditor(statusMsgDispatcher, this)},
    m_landingInfoWidget(new QWidget(this))
{
    // Setup UI
    auto layout = new QVBoxLayout(this);
    initLandingInfo();

    // Event listeners
    m_ravenTree = m_mainWindow->getRavenLHSView()->getRavenTree();
    connect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::renderDiffItem);
}

RavenRHSView::~RavenRHSView()
{
    // cleanup
    disconnect(m_ravenTree, &RavenTree::renderDiffItem, this, &RavenRHSView::renderDiffItem);
}

void RavenRHSView::renderDiffItem(GitManager::GitDiffItem item)
{
    qDebug() << "RavenRHSView::renderDiffItem called";

    // Add WebView to layout if it's first time
    if (m_showLandingInfo)
    {
        layout()->replaceWidget(m_landingInfoWidget, m_ravenEditor);
        m_landingInfoWidget->deleteLater();
        m_showLandingInfo = false;
    }

    // Render the diff
    // Note: We add a delay so QWebEngine can initialize.
    // FIXME: remove delay logic here
    QTimer::singleShot(300, [this,item]{
        m_ravenEditor->openDiffItem(std::move(item));
    });
}

void RavenRHSView::initLandingInfo()
{
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
