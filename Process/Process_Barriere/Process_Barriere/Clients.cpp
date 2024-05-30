#include "Clients.h"

Clients::Clients(QTcpSocket* socket, const QString& name)
    : socket(socket), name(name) {
}

QTcpSocket* Clients::getSocket() const {
    return socket;
}

QString Clients::getName() const {
    return name;
}