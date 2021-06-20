#include "TrayIcon.h"
#include "src/Context.h"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>

TrayIcon::TrayIcon()
    : makeScreenshotAction_("Make screenshot", this),
      updateAction_("Update", this),
      settingsAction_("Settings", this),
      quitAction_("Quit", this)
{
#if defined(Q_OS_LINUX)
    setIcon(QIcon(":/icons/icon-22.png"));
#elif defined(Q_OS_WIN32)
    setIcon(QIcon(":/icons/icon-16.png"));
#elif defined(Q_OS_MACOS)
    setIcon(QIcon(":/icons/icon-22.png"));
#endif

    setToolTip("Qsnip");

    iconMenu_.addAction(&makeScreenshotAction_);
#if defined(Q_OS_WIN32) || defined(Q_OS_MACOS)
    iconMenu_.addAction(&updateAction_);
#endif
    iconMenu_.addAction(&settingsAction_);
    iconMenu_.addAction(&quitAction_);

    setContextMenu(&iconMenu_);

    connect(&makeScreenshotAction_, &QAction::triggered, this, [&]() {
        emit screenshotActionTriggered();
    });

    // FIXME
    // connect(&updateAction_, &QAction::triggered, this, [&]() {
    //     Context::getInstance().updater.check();
    // });

    connect(&settingsAction_, &QAction::triggered, this, [&]() {
        Context::getInstance().settingsForm->show();
    });

    connect(&quitAction_, &QAction::triggered, qApp, &QCoreApplication::quit, Qt::QueuedConnection);

    connect(this, &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            emit screenshotActionTriggered();
        }
    });
}
