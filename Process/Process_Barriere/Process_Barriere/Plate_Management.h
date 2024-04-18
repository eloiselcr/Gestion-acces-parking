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
    static void AnalysePlaque(QString plaque, Ui::Process_BarriereClass& ui);
    static void GestionMode(Mode modeActif, const QString& statut, Ui::Process_BarriereClass& ui);

private slots:
    void Plate_Management::on_btnOuvrirBarriere_clicked(QString plaque, Ui::Process_BarriereClass& ui);
    
private: 
    QString nom;
    QString prenom;
    QString statut;
    int iduser;

};
