#pragma once

#include <QString>
#include <QSqlQuery>
#include <QtSql>
#include <QDateTime>
#include "ui_Process_Barriere.h" // Incluez si nécessaire

class Process_Barriere;

class Functions {

public:
    static void AnalysePlaque(QString plaque, Ui::Process_BarriereClass& ui, QString& statut);


private:
    // Autres membres privés si nécessaire...
};
