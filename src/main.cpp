#include "glwidget.h"
#include "primitive2d.h"
#include <QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    GLWidget glwidget;
    glwidget.resize(800, 600);
    glwidget.show();

    // line2d l({10, 20}, {30, 20});
    // circle2d c({41, 20}, 10);
    // collisionPrimitivesPoint point;
    // qDebug() << collisionPrimitives(l, c, point);

    // return 0;

    return app.exec();
}
