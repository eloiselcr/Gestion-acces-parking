#include "SQLQuerys.h"
#include "Plate_Management.h"
#include "Process_Barriere.h"


void Plate_Management::AnalysePlaque(QString plaque, Mode modeActif)
{
	/* AnalysePlaque() : récupère les informations de la plaque en BDD, connecte les données aux variables
	associées, les affiche sur l'ui puis envoi à GestionMode. */

	qDebug() << "\n=== Entrée dans AnalysePlaque ===\n";

	QSqlQuery queryInfos;
	queryInfos.prepare(SQLQueries::getqueryInfos(plaque));
	queryInfos.bindValue(":plaque", plaque);

	if (!queryInfos.exec()) {
		qDebug() << "Erreur lors de l'execution de la requête SQL : " << queryInfos.lastError().text();
		return;
	}

	if (queryInfos.next()) {
		id_demande = queryInfos.value(0).toInt();
		nom = queryInfos.value(1).toString();
		prenom = queryInfos.value(2).toString();
		statut = queryInfos.value(3).toString();
		plaque = queryInfos.value(4).toString();
		date = queryInfos.value(5).toDateTime();                                    
		id_user = queryInfos.value(6).toInt();

		qDebug() << "id demande : " << id_demande;
		qDebug() << "Nom : " << nom;
		qDebug() << "Prénom : " << prenom;
		qDebug() << "Plaque : " << plaque;
		qDebug() << "Statut du véhicule :" << statut;
		qDebug() << "Date effective :" << date;
		qDebug() << "id user : " << id_user;
		qDebug() << "Mode actif : " << modeActif;
		ui.label_ImmatriculationDisplay->setText(plaque);


		static const QMap<QString, QString> statutMessages = {
			{"Refusee", "Le véhicule a été refusé par l'administration."},
			{"Traitement en cours", "La demande pour ce véhicule est en cours de traitement."},
			{"Informations demandees", "Des informations supplémentaires ont été demandées pour ce véhicule."},
			{"Validee", "Ce véhicule a été validé par l'administration."},
		};

		if (gStatut() == "Validee") {
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
		if (statutMessages.contains(gStatut())) {
			ui.label_StatutVehiculeDisplay->setText(statutMessages[gStatut()]);
		}
	}
	else {
		qDebug() << "Plaque inconnue";
		statut = "Inconnue";
		ui.label_ImmatriculationDisplay->setText(plaque);
		ui.label_StatutVehiculeDisplay->setText("Plaque inconnue du système.");
	}
	setPlaque(plaque);
	GestionMode(modeActif);
}


void Plate_Management::GestionMode(Mode modeActif)
{
	/* GestionMode() : en fonction du mode actif, oriente vers une suite d'évènements.
	2 cas possible = intervention du superviseur ou envoi direct en BDD. */

	qDebug() << "\n=== Entrée dans GestionMode ===\n";
	qDebug() << "Mode : " << modeActif;
	qDebug() << "Plaque : " << gPlaque();
	switch (modeActif) {

	case CasparCas:
		if (gStatut() != "Validee") {
			ui.widget_SupervisionBarriere->setVisible(true);
		}
		else {
			DirectSendSQL(modeActif);
		}
		break;

	case GestionGlobale:
		DirectSendSQL(modeActif);
		break;

	case Manuel:
		ui.widget_SupervisionBarriere->setVisible(true);
		break;
	}
}


void Plate_Management::DirectSendSQL(Mode modeActif)
{
	/* DirectSendSQL() : envoi direct en BDD si la plaque est valide en CPC et GB. */

	qDebug() << "\n=== Entrée dans DirectSendSQL ===\n";
	qDebug() << "id demande : " << gid_Demande();
	qDebug() << "Plaque : " << gPlaque();
	qDebug() << "Statut : " << gStatut();
	qDebug() << "id user : " << gid_User();

	QSqlQuery queryAcces;
	queryAcces.prepare(SQLQueries::insertAcces());
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":id_demande", gid_Demande());

	QSqlQuery queryAccesSD;
	queryAccesSD.prepare(SQLQueries::insertAccesSD());
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", gPlaque());

	static const QMap<QString, QString> motifs = {
		{"Refusee", "Refusé"},
		{"Traitement en cours", "En cours de traitement"},
		{"Informations demandees", "Informations demandées"},
		{"Validee mais expiree", "Validité expirée"},
		{"Iconnue", "Véhicule inconnu"}
	};

	if (modeActif == CasparCas || (modeActif == GestionGlobale && gStatut() == "Validee")) {
		if (!queryAcces.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Accs : " << queryAcces.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces réussie.";
			//sendOpenBarriere();
		}
	}
	else {
		queryAccesSD.bindValue(":motif", motifs.value(gStatut()));
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande : " << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
			//sendOpenBarriere();
		}
	}
}


void Plate_Management::on_btnOuvrirBarriere_clicked(Mode modeActif)
{
	/* on_btnOuvrirBarriere_clicked() : permet au superviseur d'ouvrir la barrière en cas d'intervention. */

	qDebug() << "\n===Entrée dans on_btnOuvrirBarriere_clicked ===\n";
	qDebug() << "id demande : " << gid_Demande();
	qDebug() << "Plaque : " << gPlaque();
	qDebug() << "Statut : " << gStatut();
	qDebug() << "id user : " << gid_User();

	QSqlQuery queryAcces;
	queryAcces.prepare(SQLQueries::insertAcces());
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":id_demande", gid_Demande());

	QSqlQuery queryAccesSD;
	queryAccesSD.prepare(SQLQueries::insertAccesSD());
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", gPlaque());

	static const QMap<QString, QString> motifs = {
	{"Refusee", "Refusé"},
	{"Traitement en cours", "En cours de traitement"},
	{"Informations demandees", "Informations demandées"},
	{"Validee mais expiree", "Validité expirée"},
	{"Iconnue", "Véhicule inconnu"}
	};

	if (gStatut() == "Validee")
	{
		if (!queryAcces.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces : " << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces réussie.";
			//sendOpenBarriere();
		}
	} 
	else
	{
		queryAccesSD.bindValue(":motif", motifs.value(gStatut()));
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande : " << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
			//sendOpenBarriere();
		}
	}
}

