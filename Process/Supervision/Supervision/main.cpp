#include "Supervision.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Supervision w;
    w.show();
    return a.exec();
}
