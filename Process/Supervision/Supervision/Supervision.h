#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_Supervision.h"
#include "DatabaseConnect.h"

class Supervision : public QMainWindow
{
    Q_OBJECT

public:
    Supervision(QWidget *parent = nullptr);
    ~Supervision();

private slots:
    void on_btnAccesConnexion_clicked();
    void on_btnAskStatut_clicked();

private:
    Ui::SupervisionClass ui;
    DatabaseConnect databaseConnect;
};
