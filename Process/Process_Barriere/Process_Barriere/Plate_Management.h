#pragma once
#include <QString>
#include <QSqlQuery>
#include <QtSql>
#include <QDateTime>
#include "ui_Process_Barriere.h" 
#include "Mode.h"


class Process_Barriere;

class Plate_Management {

public:
    QString motifRef = "Vehicule refuse par l'administration.";
    QString motifDemCours = "Vehicule dont le traitement de la demande est en cours.";
    QString motifDemInfos = "Vehicule dont des informations on ete demandees.";
    QString motifExpVal = "Vehicule avec une validitee expiree";
    QString motifInco = "Vehicule iconnu";

    static void AnalysePlaque(QString plaque, Mode modeactif, Ui::Process_BarriereClass& ui);
    static void GestionMode(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);

private slots:
    void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);
    void Plate_Management::DirectSendSQL(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);
    
private: 
    QString nom;
    QString prenom;
    QString statut;
    int iduser;
};
