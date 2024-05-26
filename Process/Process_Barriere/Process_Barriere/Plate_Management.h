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
    QString statut;
    QDateTime date;
    int id_demande;
    int iduser;

    void AnalysePlaque(QString plaque, Mode modeactif, Ui::Process_BarriereClass& ui);
    void GestionMode(const int id_demande, QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);

public slots:
    void on_btnOuvrirBarriere_clicked(const int id_demande, QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);
    void DirectSendSQL(const int id_demande, QString plaque, Mode modeActif, const QString& statut, const int iduser, Ui::Process_BarriereClass& ui);

private: 
    QString nom;
    QString prenom;
};
