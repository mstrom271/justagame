#include "glwidget.h"
#include <QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    GLWidget glwidget;
    glwidget.resize(800, 600);
    glwidget.show();

    return app.exec();
}
