#include "App.h"
#include "utils/System.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    System system;
    if (system.isLaunched()) {
        qInfo() << "The application is already running";
        return 1;
    }

    // FIXME
    // QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setOrganizationName("Module Art");
    app.setOrganizationDomain("moduleart.github.io");
    app.setApplicationName("Qsnip");

    QFile file(":/styles/style.qss");
    file.open(QFile::ReadOnly);
    QString style = QLatin1String(file.readAll());
    file.close();

    app.setQuitOnLastWindowClosed(false);
    app.setStyleSheet(style);

    App application;
    return app.exec();
}
