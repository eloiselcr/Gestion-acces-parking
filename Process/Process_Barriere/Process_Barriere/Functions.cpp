#include "Functions.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

void Functions::AnalysePlaque(QString plaque, Ui::Process_BarriereClass& ui, QString& statut) {

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
        statut = statut;

        // 1 - On vérifie le statut du véhicule pour une anomalie
        if (statut == "Refusee" || statut == "Traitement en cour" || statut == "Informations demandees") {
            qDebug() << "Statut anormal : " << statut;

            if (statut == "Refusee") {
                qDebug() << "Refusee";
                ui.label_StatutVehiculeDisplay->setText("Le vehicule a ete refuse par l'administration.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
            else if (statut == "Traitement en cour") {
                qDebug() << "Traitement en cours";
                ui.label_StatutVehiculeDisplay->setText("La demande pour ce vehicule est en cours de traitement.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
            else if (statut == "Informations demandees") {
                qDebug() << "Informations demandees";
                ui.label_StatutVehiculeDisplay->setText("Des informations supplementaires ont ete demandes pour ce vehicule.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
            else if (statut == "") {
                qDebug() << "Plaque inconnue ou mal reconnue";
                ui.label_StatutVehiculeDisplay->setText("Vehicule inconnu de la base de donnees.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
        }
        else if (statut == "Validee") {
            QDateTime dateActuelle = QDateTime::currentDateTime();
            QDateTime dateLimite = dateActuelle.addYears(-1);

            if (date > dateLimite) {
                qDebug() << "Date de validite valide";
                ui.label_StatutVehiculeDisplay->setText("Vehicule valide.");
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
                // Ouvrir la barrière
            }
            else {
                qDebug() << "Date de validite expiree";
                ui.label_StatutVehiculeDisplay->setText("Vehicule avec une validite expiree.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
        }
    }
}
