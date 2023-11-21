
#include "myclass.h"
#include <QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MyClass myClass;
    myClass.resize(400, 300);
    myClass.show();

    return app.exec();
}
