#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include <QMap>
#include "Plate_Management.h"
#include "Process_Barriere.h"


void Plate_Management::AnalysePlaque(QString plaque, Mode modeActif, Ui::Process_BarriereClass& ui)
{
	/* AnalysePlaque() : récupère les informations de la plaque en BDD, connecte les données aux variables
	associées, les affiche sur l'ui puis envoi à GestionMode. */

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
		QString nom = queryInfos.value(0).toString();
		QString prenom = queryInfos.value(1).toString();
		QString statut = queryInfos.value(2).toString(); 
		QDateTime date = queryInfos.value(3).toDateTime();                                    
		int iduser = queryInfos.value(4).toInt();

		qDebug() << "Nom : " << nom;
		qDebug() << "Prenom : " << prenom;
		qDebug() << "Plaque : " << plaque;
		qDebug() << "Statut du vehicule :" << statut;
		qDebug() << "Date effective :" << date.toString();
		qDebug() << "iduser : " << iduser;
		ui.label_ImmatriculationDisplay->setText(plaque);


		static const QMap<QString, QString> statutMessages = {
			{"Refusee", "Le véhicule a été refusé par l'administration."},
			{"Traitement en cours", "La demande pour ce véhicule est en cours de traitement."},
			{"Informations demandees", "Des informations supplémentaires ont été demandées pour ce véhicule."},
			{"Validee", "Ce véhicule a été validé par l'administration."},
		}; 

		if (statutMessages.contains(statut)) {
			ui.label_StatutVehiculeDisplay->setText(statutMessages[statut]);
		}
		else if (statut == "Validee") {
			QDateTime dateActuelle = QDateTime::currentDateTime();
			QDateTime dateLimite = date.addYears(1);

			if (dateActuelle > dateLimite) {
				qDebug() << "Date de validite expiree";
				statut = "Validee mais expiree";
				ui.label_StatutVehiculeDisplay->setText("Véhicule avec une validité expirée.");
			}
			else {
				qDebug() << "Date de validite valide";
				ui.label_StatutVehiculeDisplay->setText("Véhicule validé.");
			}
		}
	}
	GestionMode(plaque, modeActif, statut, iduser, ui);
}

void Plate_Management::GestionMode(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	/* GestionMode() : en fonction du mode actif, oriente vers une suite d'évènements.
	2 cas possible = intervention du superviseur ou envoi direct en BDD. */

	qDebug() << "Entree dans le Process Gestion Mode";
	switch (modeActif) {

	case CasparCas:
		if (statut != "Validee") {
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else {
			DirectSendSQL(plaque, modeActif, statut, iduser, ui);
		}
		break;

	case GestionGlobale:
		DirectSendSQL(plaque, modeActif, statut, iduser, ui);
		break;

	case Manuel:
		ui.widget_SupervisionBarriere->setVisible(true);
		break;
	}
}

void Plate_Management::DirectSendSQL(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	/* DirectSendSQL() : envoi direct en BDD si la plaque est valide. */

	QSqlQuery queryAcces;
	queryAcces.prepare("INSERT INTO Acces (date_horaire, id_demande) VALUES (:date, :id_demande)");
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":id_demande", iduser);

	QSqlQuery queryAccesSD;
	queryAccesSD.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", plaque);

	static const QMap<QString, QString> motifs = {
		{"Refusee", "Refusé"},
		{"Traitement en cours", "En cours de traitement"},
		{"Informations demandees", "Informations demandées"},
		{"Validee mais expiree", "Validité expirée"},
		{"", "Véhicule inconnu"}
	};

	if (modeActif == CasparCas || (modeActif == GestionGlobale && statut == "Validee")) {
		if (!queryAcces.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces:" << queryAcces.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces réussie.";
		}
	}
	else {
		queryAccesSD.bindValue(":motif", motifs.value(statut, "Autre"));
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
	}
}

//void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Mode modeActif, const QString& statut, int iduser, Ui::Process_BarriereClass& ui)
//{
//	qDebug() << "=== on_btnOuvrirBarriere_clicked ===";
//	qDebug() << "Plaque:" << plaque;
//	qDebug() << "Mode Actif:" << modeActif;
//	qDebug() << "Statut:" << statut;
//	qDebug() << "ID utilisateur:" << iduser;
//
//	QSqlQuery querySupervisionAccesSD;
//	querySupervisionAccesSD.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
//	querySupervisionAccesSD.bindValue(":date", QDateTime::currentDateTime());
//	qDebug() << "Date actuelle:" << QDateTime::currentDateTime();
//	querySupervisionAccesSD.bindValue(":immatriculation", plaque);
//
//	static const QMap<QString, QString> motifs = {
//		{"Refusee", "Refusé"},
//		{"Traitement en cours", "En cours de traitement"},
//		{"Informations demandees", "Informations demandées"},
//		{"Validee mais expiree", "Validité expirée"},
//		{"", "Véhicule inconnu"}
//	};
//
//	QString motif = motifs.value(statut, "Autre");
//	qDebug() << "Motif associé au statut:" << motif;
//	querySupervisionAccesSD.bindValue(":motif", motif);
//
//	if (!querySupervisionAccesSD.exec()) {
//		qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << querySupervisionAccesSD.lastError().text();
//	}
//	else {
//		qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
//	}
//}



/*void Plate_Management::GestionMode(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	switch (modeActif) {
		// Cette fonction oriente vers les bonnes fonctions en fonction du Mode actif et du Statut.
		qDebug() << "Entree dans le Process Gestion Mode";

	case CasparCas:
		if (statut == "Refusee" || "Traitement en cours" || "Informations demandees" || "" || "Validee mais expiree") {
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else {
			DirectSendSQL(plaque, modeActif, statut, iduser, ui);
		}
		break;

	case GestionGlobale:
		DirectSendSQL(plaque, modeActif, statut, iduser, ui);
		break;

	case Manuel:
		ui.widget_SupervisionBarriere->setVisible(true);
		break;
	}
}


// DirectSendSQL envoi en BDD les plaques qui rentrent en BDD sans intervention de la Supervision
void Plate_Management::DirectSendSQL(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	// On fait une queryAcces Plaque Validee en mode : Cas-par-Cas + Gestion Globale
	// On fait une queryAccesSD Plaque non Validee en mode : GestionGlobale 

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
			queryAccesSD.bindValue(":motif", motifRefus);
		}
		else if (statut == "Traitement en cours") {
			queryAccesSD.bindValue(":motif", motifDemCours);
		}
		else if (statut == "Informations demande") {
			queryAccesSD.bindValue(":motif", motifDemInfos);
		}
		else if (statut == "Validee mais expiree") {
			queryAccesSD.bindValue(":motif", motifValidExp);
		}
		else if (statut == "") {
			queryAccesSD.bindValue(":motif", motifVehInco);
		}

		// ON ENVOI LA REQUETE
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
		break;
	}
}


// Si intervention du superviseur avec le bouton 
void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Mode modeActif, const QString& statut, int iduser, Ui::Process_BarriereClass& ui)
{
	QSqlQuery querySupervisionAcces;
	querySupervisionAcces.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	querySupervisionAcces.bindValue(":date", QDateTime::currentDateTime());
	querySupervisionAcces.bindValue(":immatriculation", plaque);

	QSqlQuery querySupervisionAccesSD;
	querySupervisionAccesSD.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
	querySupervisionAccesSD.bindValue(":date", QDateTime::currentDateTime());
	querySupervisionAccesSD.bindValue(":immatriculation", plaque);

	switch (modeActif) {

	case CasparCas:
		if (statut == "Refusee") {
			querySupervisionAccesSD.bindValue(":motif", motifRefus);
		}
		else if (statut == "Traitement en cours") {
			querySupervisionAccesSD.bindValue(":motif", motifDemCours);
		}
		else if (statut == "Informations demande") {
			querySupervisionAccesSD.bindValue(":motif", motifDemInfos);
		}
		else if (statut == "Validee mais expiree") {
			querySupervisionAccesSD.bindValue(":motif", motifValidExp);
		}
		else if (statut == "") {
			querySupervisionAccesSD.bindValue(":motif", motifVehInco);
		}
		// ON ENVOI LA REQUETE
		if (!querySupervisionAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << querySupervisionAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
		break;

	case Manuel:
		if (statut == "Validee") {
			if (statut == "Valide") {
				if (!querySupervisionAcces.exec()) {
					qDebug() << "Erreur lors de l'insertion dans la table Acces:" << querySupervisionAcces.lastError().text();
				}
			}
		}
		if (statut == "Refusee") {
			querySupervisionAccesSD.bindValue(":motif", motifRefus);
		}
		else if (statut == "Traitement en cours") {
			querySupervisionAccesSD.bindValue(":motif", motifDemCours);
		}
		else if (statut == "Informations demande") {
			querySupervisionAccesSD.bindValue(":motif", motifDemInfos);
		}
		else if (statut == "Validee mais expiree") {
			querySupervisionAccesSD.bindValue(":motif", motifValidExp);
		}
		else if (statut == "") {
			querySupervisionAccesSD.bindValue(":motif", motifVehInco);
		}
		// ON ENVOI LA REQUETE
		if (!querySupervisionAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << querySupervisionAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
		break;
	}
}
*/



