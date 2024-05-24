#include <QtWidgets/QApplication>
#include "Process_Barriere.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Process_Barriere w;
    w.show();
    return a.exec();
}
