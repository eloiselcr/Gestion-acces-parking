#pragma once
#include <QString>
#include <QDateTime>
#include "ui_Process_Barriere.h" 
#include "Mode.h"

class Plate_Management 
{

public:
    Plate_Management(Ui::Process_BarriereClass& ui) : ui(ui) {}
    Ui::Process_BarriereClass& ui;

    void AnalysePlaque(QString plaque, Mode modeActif);
    void GestionMode(Mode modeActif);
    void setPlaque(QString plaque) { this->plaque = plaque; };
    
    QString gPlaque() { return this->plaque; };
    QString gStatut() { return this->statut; };
    QDateTime gDate() { return this->date; };
    int gid_Demande() { return this->id_demande; };
    int gid_User() { return this->id_user; };

public slots:
    void on_btnOuvrirBarriere_clicked(Mode modeActif);
    void DirectSendSQL(Mode modeActif);

private: 
    QString nom;
    QString prenom;
    QString plaque;
    QString statut;
    QDateTime date;
    int id_demande;
    int id_user;
    Mode modeActif;
};
