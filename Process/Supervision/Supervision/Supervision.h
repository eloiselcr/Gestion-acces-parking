#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QtWidgets/QMainWindow>
#include "ui_Supervision.h"
#include "DatabaseConnect.h"
#include "Clients.h"

class Supervision : public QMainWindow {

    Q_OBJECT

public:
    Supervision(QWidget *parent = nullptr);
    ~Supervision();

private slots:
    void on_btnAccesConnexion_clicked();
    void on_btnAskStatut_clicked();
    void prout();
    //void sendEtatCamera();
    //void sendEtatArd1();
    //void sendEtatArd2();

    //void onClientConnected();
    //void onClientReadyRead();
    //void interractClient(Clients* client, const QJsonObject& jsonMessage);

private:
    Ui::SupervisionClass ui;
    DatabaseConnect databaseConnect;
    QTcpServer* server; // Déclaration du serveur TCP/IP
    QTcpSocket* clientConnection; // Déclaration du socket pour la connexion avec le client

    QMap<QTcpSocket*, Clients*> clients; 
    QMap<QString, QString> clientsConnus;
    
};
