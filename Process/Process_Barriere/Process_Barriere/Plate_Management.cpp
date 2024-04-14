#include "Plate_Management.h"
#include "Process_Barriere.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

void Plate_Management::AnalysePlaque(QString plaque, Ui::Process_BarriereClass& ui) {

	QSqlQuery query1;
	QSqlQuery query2;

	query1.prepare("SELECT nom, prenom, statut, date, iduser FROM Demande_Vehicule WHERE immatriculation = :plaque");
	query1.bindValue(":plaque", plaque);

	if (!query1.exec()) {
		qDebug() << "Erreur lors de l'execution de la requete SQL:" << query1.lastError().text();
		return;
	}

	if (query1.next()) {
		QString nom = query1.value(0).toString(); // récupère le nom
		QString prenom = query1.value(1).toString(); // récupère le prenom
		QString statut = query1.value(2).toString(); // récupère le statut
		QDateTime date = query1.value(3).toDateTime(); // récupère la date
		int iduser = query1.value(4).toInt(); // récupère l'iduser

		qDebug() << "Nom : " << nom;
		qDebug() << "Prenom : " << prenom;
		qDebug() << "Plaque : " << plaque;
		qDebug() << "Statut du vehicule :" << statut;
		qDebug() << "Date effective :" << date.toString();
		qDebug() << "iduser : " << iduser;
		ui.label_ImmatriculationDisplay->setText(plaque);

		if (statut == "Refusee") {
			ui.label_StatutVehiculeDisplay->setText("Le vehicule a ete refuse par l'administration.");
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "Traitement en cour") {
			ui.label_StatutVehiculeDisplay->setText("La demande pour ce vehicule est en cours de traitement.");
			// ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "Informations demandees") {
			ui.label_StatutVehiculeDisplay->setText("Des informations supplementaires ont ete demandes pour ce vehicule.");
			// ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "") {
			qDebug() << "Plaque inconnue ou mal reconnue";
			ui.label_StatutVehiculeDisplay->setText("Vehicule inconnu de la base de donnees.");
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "Validee") {
			QDateTime dateActuelle = QDateTime::currentDateTime();
			QDateTime dateLimite = dateActuelle.addYears(-1);

			if (date > dateLimite) {
				qDebug() << "Date de validite valide";
				ui.label_StatutVehiculeDisplay->setText("Vehicule valide.");
			}
			else {
				qDebug() << "Date de validite expiree";
				ui.label_StatutVehiculeDisplay->setText("Vehicule avec une validite expiree.");
				ui.widget_SupervisionBarriere->setVisible(true);
			}
		}
	}
}


/*void GestionMode(Process_Barriere::Mode modeActif, Ui::Process_BarriereClass& ui)
{
	switch (modeActif) {

	case Process_Barriere::CasparCas:
		break;

	case Process_Barriere::GestionGlobale:
		break;

	case Process_Barriere::Manuel:
		break;
	}
}
*/


// DANS TOUS LES CAS, IL FAUT ENREGISTRER LE MOTIF DE PROBLEME, ON AFFICHE PAS ENCORE LA SUPERVISION DANS MOTIF


void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Ui::Process_BarriereClass& ui) {
	// Déterminer le motif de problème en fonction du statut
	QString motif;
	qDebug() << "Entree dans le mode Supervision";
	qDebug() << "Statut du vehicule:" << statut;


	if (statut == "Refusee") {
		motif = "Vehicule refuse par l'administration";
	}
	else if (statut == "Traitement en cour") {
		motif = "Demande en cours de traitement";
	}
	else if (statut == "Informations demande") {
		motif = "Informations supplementaires demandees";
	}
	else if (statut == "Plaque inconnue") {
		motif = "Plaque inconnue dans la base de donnees";
	}
	else if (statut == "Validee") { // en sachant que le bouton n'est visible que si plaque = validité expirée
		motif = "Validite de la plaque expiree";
	}

	// Envoyer la requête d'insertion dans la table "Acces_SansDemande"
	QSqlQuery query;
	query.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	query.bindValue(":date", QDateTime::currentDateTime());
	query.bindValue(":immatriculation", plaque);
	query.bindValue(":motif", motif);

	if (!query.exec()) {
		qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << query.lastError().text();
	}
	else {
		qDebug() << "Insertion dans la table Acces_SansDemande reussie.";
		// Ouvrir la barrière
	}
}

/*
query2.prepare("INSERT INTO Acces (date_horaire, id_demande) VALUES (:date, :id_demande)");
query2.bindValue(":date", QDateTime::currentDateTime());
query2.bindValue(":id_demande", iduser);
if (!query2.exec()) { // Exécuter la requête d'insertion
	qDebug() << "Erreur lors de l'insertion dans la table Acces:" << query2.lastError().text();
}
else {
	qDebug() << "Insertion ok";
}
*/

