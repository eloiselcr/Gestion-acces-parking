#include "Process_Barriere.h"

Process_Barriere::Process_Barriere(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Connexion à la base de données
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.65.167");
    db.setDatabaseName("parking");
    db.setUserName("root");
    db.setPassword("root");

    // Vérification si la BDD est accessible
    if (!db.open()) {
        ui.label_BDDVerif->setText("Non");
    }
    else {
        ui.label_BDDVerif->setText("Oui");
    }
}

Process_Barriere::~Process_Barriere()
{
    // Destructeur
}

void Process_Barriere::on_btnAccesConnexion_clicked() // Vérification Connexion
{
    QString login = ui.edit_Login->text();
    QString mdp = ui.edit_Mdp->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
    query.bindValue(":login", login);
    query.bindValue(":mdp", mdp);
    query.exec();

    if (query.next()) { // Vérification des identifians
        ui.label_ErrorConnect->setText("Identifiants corrects !");
    }
    else {
        ui.label_ErrorConnect->setText("Identifiants incorrects");
    }
}


