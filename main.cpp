#include <QApplication>
#include <QDebug>
#include "editorview.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    EditorView window;
    return app.exec();
}
