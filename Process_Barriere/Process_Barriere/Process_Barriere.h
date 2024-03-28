#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Process_Barriere.h"
#include "DatabaseConnect.h"

class Process_Barriere : public QMainWindow
{
    Q_OBJECT

public:
    Process_Barriere(QWidget* parent = nullptr);
    ~Process_Barriere();

private slots:
    void on_btnAccesConnexion_clicked();

private:
    Ui::Process_BarriereClass ui;
    DatabaseConnect databaseConnect;
};
