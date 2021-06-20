#pragma once

#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit TrayIcon();

signals:
    void screenshotActionTriggered();

private:
    QMenu iconMenu_;

    QAction makeScreenshotAction_;
    QAction updateAction_;
    QAction settingsAction_;
    QAction quitAction_;
};
