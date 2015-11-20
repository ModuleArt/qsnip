#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include "App.h"
#include "AppView.h"

AppView::AppView(App& app)
    : app_(app),
      toolbar_(*this),
      visibleAreaMode_(0),
      lineMode_(scene_),
      arrowMode_(scene_),
      rectMode_(scene_),
      ellipseMode_(scene_),
      textMode_(scene_, *this) {

    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    setScene(&scene_);
}

AppView::~AppView() {
    delete visibleAreaMode_;
}

App& AppView::app() const {
    return app_;
}

void AppView::makeScreenshot() {
    if (!app_.connected()) {
        app_.settingsForm().showCantConnect();
        return;
    }

    // Making screenshot
    QDesktopWidget* desktop = QApplication::desktop();
    QRect geo = desktop->screenGeometry(desktop->screenNumber(QCursor::pos()));

    screenshot_ = QGuiApplication::primaryScreen()->grabWindow(desktop->winId(),
                                                               geo.left(),
                                                               geo.top(),
                                                               geo.width(),
                                                               geo.height());

    int width = screenshot_.width(),
        height = screenshot_.height();

    setGeometry(0, 0, width, height);
    scene_.setSceneRect(0, 0, width, height);

    reinitVisibleArea();

    // Background screenshot
    scene_.addPixmap(screenshot_);
    showFullScreen();
}

QGraphicsScene& AppView::scene() {
    return scene_;
}

VisibleAreaMode& AppView::visibleAreaMode() const {
    return *visibleAreaMode_;
}

void AppView::setMode(const ToolbarMode mode) {
    emit toolChanged(mode);

    switch (mode) {
        case ToolbarMode::VISIBLE_AREA:
            currentMode_ = visibleAreaMode_;
            break;

        case ToolbarMode::LINE:
            currentMode_ = &lineMode_;
            app().history().lastTool(ToolbarMode::LINE);
            break;

        case ToolbarMode::ARROW:
            currentMode_ = &arrowMode_;
            app().history().lastTool(ToolbarMode::ARROW);
            break;

        case ToolbarMode::RECT:
            currentMode_ = &rectMode_;
            app().history().lastTool(ToolbarMode::RECT);
            break;

        case ToolbarMode::ELLIPSE:
            currentMode_ = &ellipseMode_;
            app().history().lastTool(ToolbarMode::ELLIPSE);
            break;

        case ToolbarMode::TEXT:
            currentMode_ = &textMode_;
            app().history().lastTool(ToolbarMode::TEXT);
            break;
    }
}

void AppView::reinitVisibleArea() {
    if (0 != visibleAreaMode_) {
        delete visibleAreaMode_;
    }

    visibleAreaMode_ = new VisibleAreaMode(scene_, toolbar_);
    currentMode_ = visibleAreaMode_;
    toolbar_.select(ToolbarMode::VISIBLE_AREA);
    setMouseTracking(true);
}

void AppView::initShortcut() {
#if defined(Q_OS_LINUX)
    shortcut_.setShortcut(QKeySequence(tr("Alt+Print")));

    connect(&shortcut_, SIGNAL(activated()),
            this, SLOT(makeScreenshot()));
#elif defined(Q_OS_WIN32)
    RegisterHotKey((HWND)winId(), 100, MOD_ALT, VK_SNAPSHOT);
#endif
}

Toolbar& AppView::toolbar() {
    return toolbar_;
}

void AppView::mousePressEvent(QMouseEvent* e) {
    int x = e->x();
    int y = e->y();

    if (currentMode_ != visibleAreaMode_
        && visibleAreaMode_->initialized()
        && visibleAreaMode_->isResizablePosition(x, y))
    {
        visibleAreaMode_->resizeInit(x, y);
        return;
    }

    currentMode_->init(x, y);
}

void AppView::mouseMoveEvent(QMouseEvent* e) {
    int x = e->x();
    int y = e->y();

    if (visibleAreaMode_->resizing()) {
        visibleAreaMode_->resizeMove(x, y);
        return;
    }

    currentMode_->move(x, y);
}

void AppView::mouseReleaseEvent(QMouseEvent* e) {
    int x = e->x();
    int y = e->y();

    if (visibleAreaMode_->resizing()) {
        visibleAreaMode_->resizeStop(x, y);
        return;
    }

    currentMode_->stop(x, y);
}

void AppView::wheelEvent(QWheelEvent* e) {
    if (e->delta() < 0) {
        toolbar_.setSelectedNext();
    } else {
        toolbar_.setSelectedPrevious();
    }
}

void AppView::keyReleaseEvent(QKeyEvent* e) {
    int key = e->key();

    if (key == Qt::Key_Escape) {
        hide();
        toolbar_.hide();
    } else if (key == Qt::Key_Return) {
        if (currentMode_ != &textMode_ || textMode_.textArea() == 0 || !textMode_.textArea()->hasFocus()) {
            toolbar_.submit(e->modifiers().testFlag(Qt::AltModifier));
        }
    }

    QGraphicsView::keyReleaseEvent(e);
}

#ifdef Q_OS_WIN32
bool AppView::nativeEvent(const QByteArray& eventType, void* message, long* result) {
    MSG* msg = reinterpret_cast<MSG*>(message);

    if (msg->message == WM_HOTKEY){
        if (msg->wParam == 100){
            makeScreenshot();
            return true;
        }
    }

    return false;
}
#endif
