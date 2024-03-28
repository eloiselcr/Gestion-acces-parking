#include "DatabaseConnect.h"

DatabaseConnect::DatabaseConnect()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.65.167");
    db.setDatabaseName("parking");
    db.setUserName("root");
    db.setPassword("root");

    // Vérification si la BDD est accessible
    if (!db.open()) {
        qDebug() << "Connexion a la base de donnees echouee";
    }
    else {
        qDebug() << "Connexion a la base de données reussie";
    }
}

bool DatabaseConnect::isConnected() const
{
    return db.isOpen();
}

