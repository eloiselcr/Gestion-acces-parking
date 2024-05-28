#pragma once
#include <QString>
#include <QSqlQuery>
#include <QtSql>
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
    void setplaque(QString plaque) { this->plaque = plaque; };
    
    QString getplaque() { return this->plaque; };
    QString getstatut() { return this->statut; };
    QDateTime getdate() { return this->date; };
    int getid_demande() { return this->id_demande; };
    int getiduser() { return this->iduser; };

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
    int iduser;
    Mode modeActif;
};
