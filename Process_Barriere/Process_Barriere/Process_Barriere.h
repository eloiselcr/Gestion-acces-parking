#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_Process_Barriere.h"
#include "DatabaseConnect.h"
#include <QTcpServer>
#include <QTcpSocket>

class Process_Barriere : public QMainWindow
{
    Q_OBJECT

public:
    Process_Barriere(QWidget* parent = nullptr);
    ~Process_Barriere();

private slots:
    void on_btnAccesConnexion_clicked();
    void on_BtnAccueilGestionGlobale_cliked();

    void onClientConnected();
    void sendLicensePlateRequest();
    void onClientReadyRead();

    void on_btnCasparCas_cliked();

private:
    Ui::Process_BarriereClass ui;
    DatabaseConnect databaseConnect;
    QTcpServer* server; // Déclaration du serveur TCP/IP
    QTcpSocket* clientConnection; // Déclaration du socket pour la connexion avec le client
    QString plaque;
};
