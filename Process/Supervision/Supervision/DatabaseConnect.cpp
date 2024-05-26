#include "DatabaseConnect.h"

DatabaseConnect::DatabaseConnect()
{
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.1.156");
	db.setDatabaseName("parking");
	db.setUserName("root");
	db.setPassword("root");

	// Vérification si la BDD est accessible
	if (!db.open()) {
		qDebug() << "Connexion a la base de donnees echouee";
	}
	else {
		qDebug() << "Connexion a la base de donnees reussie";
	}
}
