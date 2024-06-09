#pragma once
#include <QTcpSocket>
#include <QString>

class Clients 
{

public:
    Clients(QTcpSocket* socket, const QString& name);
    QTcpSocket* getSocket() const;
    QString getName() const;

private:
    QTcpSocket* socket;
    QString name;
};


