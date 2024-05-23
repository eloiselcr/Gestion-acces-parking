#include "Plate_Management.h"
#include "Process_Barriere.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include "Process_Barriere.h"


void Plate_Management::AnalysePlaque(QString plaque, Mode modeActif, Ui::Process_BarriereClass& ui)
{
	qDebug() << "\n=== DEBUT ANALYSE PLAQUE ===";
	QSqlQuery queryInfos;

	queryInfos.prepare("SELECT nom, prenom, statut, date, iduser FROM Demande_Vehicule WHERE immatriculation = :plaque");
	queryInfos.bindValue(":plaque", plaque);
	qDebug() << "Requete SQL preparee : " << queryInfos.lastQuery();

	if (!queryInfos.exec()) {
		qDebug() << "Erreur lors de l'execution de la requete SQL : " << queryInfos.lastError().text();
		return;
	}

	if (queryInfos.next()) {
		QString nom = queryInfos.value(0).toString(); // récupère le nom
		QString prenom = queryInfos.value(1).toString(); // récupère le prenom
		QString statut = queryInfos.value(2).toString(); // récupère le statut
		QDateTime date = queryInfos.value(3).toDateTime(); // récupère la date                                      
		int iduser = queryInfos.value(4).toInt(); // récupère l'iduser

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
		else if (statut == "Traitement en cours") {
			ui.label_StatutVehiculeDisplay->setText("La demande pour ce vehicule est en cours de traitement.");
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "Informations demandees") {
			ui.label_StatutVehiculeDisplay->setText("Des informations supplementaires ont ete demandes pour ce vehicule.");
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "") {
			qDebug() << "Plaque inconnue ou mal reconnue";
			ui.label_StatutVehiculeDisplay->setText("Vehicule inconnu de la base de donnees.");
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else if (statut == "Validee") {
			QDateTime dateActuelle = QDateTime::currentDateTime(); // date actuelle
			QDateTime dateLimite = dateActuelle.addYears(-1);

			if (dateActuelle > dateLimite) { // Comparaison inversée
				qDebug() << "Date de validite expiree";
				statut = "Validee mais expiree";
				ui.label_StatutVehiculeDisplay->setText("Vehicule avec une validite expiree.");
				ui.widget_SupervisionBarriere->setVisible(true);
			}
			else {
				qDebug() << "Date de validite valide";
				ui.label_StatutVehiculeDisplay->setText("Vehicule valide.");
			}
		}
		GestionMode(plaque, modeActif, statut, iduser, ui);
	}
}


void Plate_Management::GestionMode(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	switch (modeActif) {
		// Cette fonction oriente vers les bonnes fonctions en fonction du Mode actif et du Statut.
		qDebug() << "Entree dans le Process Gestion Mode";

	case CasparCas:
		if (statut == "Refusee" || "Traitement en cours" || "Informations demandees" || "" || "Validee mais expiree") {
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else {
			//DirectSendSQL(plaque, modeActif, statut, iduser, ui);
		}
		break;

	case GestionGlobale:
		// DirectSendSQL;
		break;

	case Manuel:
		ui.widget_SupervisionBarriere->setVisible(true);
		break;
	}
}


void Plate_Management::DirectSendSQL(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	// On fait une query1 Valide pour le Cas par Cas et le Valide de GestionGlobale
	// On fait une query2 NonValide pour la GestionGlobale

	QSqlQuery queryAcces;
	queryAcces.prepare("INSERT INTO Acces (date_horaire, id_demande) VALUES (:date, :id_demande)");
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":immatriculation", plaque);
	queryAcces.bindValue(":id_demande", iduser);

	QSqlQuery queryAccesSD;
	queryAccesSD.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", plaque);



	switch (modeActif) {

	case CasparCas:
		if (!queryAcces.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces:" << queryAcces.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces réussie.";
		}
		break;

	case GestionGlobale:
		if (statut == "Valide") {
			if (!queryAcces.exec()) {
				qDebug() << "Erreur lors de l'insertion dans la table Acces:" << queryAcces.lastError().text();
			}
		}
		else if (statut == "Refusee") {
			queryAccesSD.bindValue(":motif", motifRef);
		}
		else if (statut == "Traitement en cours") {
			queryAccesSD.bindValue(":motif", motifDemCours);
		}
		else if (statut == "Informations demande") {
			queryAccesSD.bindValue(":motif", motifDemInfos);
		}
		else if (statut == "Validee mais expiree") {
			queryAccesSD.bindValue(":motif", motifExpVal);
		}
		else if (statut == "") {
			queryAccesSD.bindValue(":motif", motifInco);
		}
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
			// Code supplémentaire si nécessaire après l'insertion réussie
		}
		break;

	}
	// On exécute
}



// ICI ON GERE LES ACCES NON AUTORISES
void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Mode modeActif, const QString& statut, int iduser, Ui::Process_BarriereClass& ui)
{
	QSqlQuery query;
	query.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	query.bindValue(":date", QDateTime::currentDateTime());
	query.bindValue(":immatriculation", plaque);

	switch (modeActif) {

	case CasparCas:
		if (statut == "Refusee") {
			query.bindValue(":motif", motifRef);
		}
		else if (statut == "Traitement en cours") {
			query.bindValue(":motif", motifDemCours);
		}
		else if (statut == "Informations demande") {
			query.bindValue(":motif", motifDemInfos);
		}
		else if (statut == "Validee mais expiree") {
			query.bindValue(":motif", motifExpVal);
		}
		else if (statut == "") {
			query.bindValue(":motif", motifInco);
		}
		break;

	case GestionGlobale:

		break;

	case Manuel:
		break;
	}
}





	/*
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
	else if (statut == "") {
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
	*/


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

