#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ClientSimulation.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ClientSimulation llama;
    return a.exec();
}
