#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include <QMap>
#include "querys.h"
#include "Plate_Management.h"
#include "Process_Barriere.h"


void Plate_Management::AnalysePlaque(QString plaque, Mode modeActif, Ui::Process_BarriereClass& ui)
{
	/* AnalysePlaque() : récupère les informations de la plaque en BDD, connecte les données aux variables
	associées, les affiche sur l'ui puis envoi à GestionMode. */

	qDebug() << "\n=== DEBUT ANALYSE PLAQUE ===";

	QSqlQuery queryInfos;
	queryInfos.prepare(SQLQueries::getqueryInfos(plaque));
	queryInfos.bindValue(":plaque", plaque);

	if (!queryInfos.exec()) {
		qDebug() << "Erreur lors de l'execution de la requete SQL : " << queryInfos.lastError().text();
		return;
	}

	if (queryInfos.next()) {
		id_demande = queryInfos.value(0).toInt();
		nom = queryInfos.value(1).toString();
		prenom = queryInfos.value(2).toString();
		statut = queryInfos.value(3).toString(); 
		date = queryInfos.value(4).toDateTime();                                    
		iduser = queryInfos.value(5).toInt();

		qDebug() << "id demande : " << id_demande;
		qDebug() << "Nom : " << nom;
		qDebug() << "Prenom : " << prenom;
		qDebug() << "Plaque : " << plaque;
		qDebug() << "Statut du vehicule :" << statut;
		qDebug() << "Date effective :" << date;
		qDebug() << "iduser : " << iduser;
		ui.label_ImmatriculationDisplay->setText(plaque);


		static const QMap<QString, QString> statutMessages = {
			{"Refusee", "Le véhicule a été refusé par l'administration."},
			{"Traitement en cours", "La demande pour ce véhicule est en cours de traitement."},
			{"Informations demandees", "Des informations supplémentaires ont été demandées pour ce véhicule."},
			{"Validee", "Ce véhicule a été validé par l'administration."},
		};

		if (statut == "Validee") {
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
		if (statutMessages.contains(statut)) {
			ui.label_StatutVehiculeDisplay->setText(statutMessages[statut]);
		}
	}
	else {
		qDebug() << "Plaque inconnue";
		statut = "Inconnue";
		ui.label_ImmatriculationDisplay->setText(plaque);
		ui.label_StatutVehiculeDisplay->setText("Plaque inconnue du système.");
	}
	GestionMode(id_demande, plaque, modeActif, statut, iduser, ui);
}

void Plate_Management::GestionMode(const int id_demande, QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
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
			DirectSendSQL(id_demande, plaque, modeActif, statut, iduser, ui);
		}
		break;

	case GestionGlobale:
		DirectSendSQL(id_demande, plaque, modeActif, statut, iduser, ui);
		break;

	case Manuel:
		ui.widget_SupervisionBarriere->setVisible(true);
		break;
	}
}

void Plate_Management::DirectSendSQL(const int id_demande, QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui)
{
	/* DirectSendSQL() : envoi direct en BDD si la plaque est valide en CPC et GB. */

	QSqlQuery queryAcces;
	queryAcces.prepare(SQLQueries::insertAcces());
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":id_demande", id_demande);

	QSqlQuery queryAccesSD;
	queryAccesSD.prepare(SQLQueries::insertAccesSD());
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", plaque);

	static const QMap<QString, QString> motifs = {
		{"Refusee", "Refusé"},
		{"Traitement en cours", "En cours de traitement"},
		{"Informations demandees", "Informations demandées"},
		{"Validee mais expiree", "Validité expirée"},
		{"Iconnue", "Véhicule inconnu"}
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
		queryAccesSD.bindValue(":motif", motifs.value(statut));
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
	}
}

void Plate_Management::on_btnOuvrirBarriere_clicked(const int id_demande, QString plaque, Mode modeActif, const QString& statut, int iduser, Ui::Process_BarriereClass& ui)
{
	/* on_btnOuvrirBarriere_clicked() : */

	qDebug() << "=== on_btnOuvrirBarriere_clicked ===";
	qDebug() << "id demande : " << id_demande;
	qDebug() << "Plaque : " << plaque;
	qDebug() << "Mode actif : " << modeActif;
	qDebug() << "Statut : " << statut;
	qDebug() << "id utilisateur : " << iduser;

	QSqlQuery queryAcces; // MANUEL : Valide 
	queryAcces.prepare(SQLQueries::insertAcces());
	queryAcces.bindValue(":date", QDateTime::currentDateTime());
	queryAcces.bindValue(":id_demande", id_demande);

	QSqlQuery queryAccesSD; // Cas-par-Cas : non ok, Manuel : non-ok
	queryAccesSD.prepare(SQLQueries::insertAccesSD());
	queryAccesSD.bindValue(":date", QDateTime::currentDateTime());
	queryAccesSD.bindValue(":immatriculation", plaque);

	static const QMap<QString, QString> motifs = {
	{"Refusee", "Refusé"},
	{"Traitement en cours", "En cours de traitement"},
	{"Informations demandees", "Informations demandées"},
	{"Validee mais expiree", "Validité expirée"},
	{"Iconnue", "Véhicule inconnu"}
	};
	qDebug() << "prout : " << statut;
	if (statut == "Validee")
	{
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
	} 
	else
	{
		queryAccesSD.bindValue(":motif", motifs.value(statut));
		if (!queryAccesSD.exec()) {
			qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << queryAccesSD.lastError().text();
		}
		else {
			qDebug() << "Insertion dans la table Acces_SansDemande réussie.";
		}
	}
}





