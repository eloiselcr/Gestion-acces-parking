#pragma once

#include <QString>
#include <QSqlQuery>
#include <QtSql>
#include <QDateTime>
#include "ui_Process_Barriere.h" // Incluez si n�cessaire

class Process_Barriere;

class Functions {

public:
    static void AnalysePlaque(QString plaque, Ui::Process_BarriereClass& ui, QString& statut);


private:
    // Autres membres priv�s si n�cessaire...
};
