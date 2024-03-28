#include "Process_Barriere.h"

Process_Barriere::Process_Barriere(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.menuBar_Modes->setVisible(false);

    // Vérification si la BDD est accessible
    /*if (DatabaseConnect.isConnected()) { // A REGLER
        ui.label_BDDVerif->setText("Non");
    }
    else {
        ui.label_BDDVerif->setText("Oui");
    }*/
}

Process_Barriere::~Process_Barriere()
{
    // Destructeur
}

void Process_Barriere::on_btnAccesConnexion_clicked() // Vérification Connexion
{
    QString login = ui.edit_Login->text();
    QString mdp = ui.edit_Mdp->text();
    ui.edit_Mdp->setEchoMode(QLineEdit::Password);

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
    query.bindValue(":login", login);
    query.bindValue(":mdp", mdp);
    query.exec();

    if (query.next()) { // Vérification des identifians
        ui.label_ErrorConnect->setText("Identifiants corrects !");
        ui.widget_Accueil->hide();
        ui.menuBar_Modes->setVisible(true);
    }
    else {
        ui.label_ErrorConnect->setText("Identifiants incorrects");
    }
}


