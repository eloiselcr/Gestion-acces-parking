#include "Supervision.h"

Supervision::Supervision(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Paramètres de visibilité graphiques
    ui.stackedWidget->setCurrentIndex(0);
    ui.edit_Mdp->setEchoMode(QLineEdit::Password);
}

Supervision::~Supervision()
{

}


// ==== PARTIE Accueil ====


void Supervision::on_btnAccesConnexion_clicked()
{
	/* on_btnAccesConnexion_clicked() : vérifie en BDD si l'utilisateur a les permissions
	nécessaires pour se connecter, ainsi que les bons identifiants. */

	QString login = ui.edit_Login->text();
	QString mdp = ui.edit_Mdp->text();

	QSqlQuery query;
	query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
	query.bindValue(":login", login);
	query.bindValue(":mdp", mdp);
	query.exec();

	qDebug() << "Requete SQL efffectuee";

	if (query.next()) {
		ui.label_ErrorConnect->setText("Identifiants corrects !");
		ui.stackedWidget->setCurrentIndex(1);
		ui.label_BienvenueTexte->setVisible(false);

		qDebug() << "ID corrects";
	}
	else {
		ui.label_ErrorConnect->setText("Identifiants incorrects");

		qDebug() << "ID incorrects";
	}
}


// ==== PARTIE Supervision ====


void Supervision::on_btnAskStatut_clicked()
{
	/* on_btnAskStatut_clicked() : envoi une demande d'actualisation des status des équipements. */
}