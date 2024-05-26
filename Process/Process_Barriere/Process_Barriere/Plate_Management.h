#pragma once
#include <QString>
#include <QSqlQuery>
#include <QtSql>
#include <QDateTime>
#include "ui_Process_Barriere.h" 
#include "Mode.h"


struct AnalyseContext {
    QString plaque;
    Mode modeActif;
    QString statut;
    int iduser;
    Ui::Process_BarriereClass& ui;

    AnalyseContext(QString p, Mode m, Ui::Process_BarriereClass& u)
        : plaque(p), modeActif(m), statut(""), iduser(-1), ui(u) {}
};

class Plate_Management
{
public:
    QString motifRefus = "Vehicule refuse par l'administration.";
    QString motifDemCours = "Vehicule dont le traitement de la demande est en cours.";
    QString motifDemInfos = "Vehicule dont des informations on ete demandees.";
    QString motifValidExp = "Vehicule avec une validitee expiree";
    QString motifVehInco = "Vehicule inconnu";

    void AnalysePlaque(QString plaque, Mode modeactif, Ui::Process_BarriereClass& ui);
    void GestionMode(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);

public slots:
    void on_btnOuvrirBarriere_clicked(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);
    void DirectSendSQL(QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);

private: 
    QString nom;
    QString prenom;
    QString statut;
    int iduser;
};
