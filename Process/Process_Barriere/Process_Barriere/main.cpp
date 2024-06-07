#include <QtWidgets/QApplication>
#include "Process_Barriere.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Process_Barriere::getInstance()->show();
    return a.exec();
}
