#pragma once
#include <QtWidgets/QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "ui_Process_Barriere.h"
#include "DatabaseConnect.h"
#include "Plate_Management.h"
#include "Mode.h"
#include "Clients.h"

class Process_Barriere : public QMainWindow {
	
	Q_OBJECT

public:
	Process_Barriere(QWidget* parent = nullptr);
	~Process_Barriere();

	void sendOpenBarriere();

	QString plaque;
	QString barriere;
	Mode modeActif;

private slots:
	void on_btnAccesConnexion_clicked();

	void onClientConnected();
	void onClientReadyRead();
	void onClientDisconnected();
	void sendLicensePlateRequest();
	void interractClient(Clients* client, const QJsonObject& jsonMessage);

	void on_btnCasparCas_cliked();
	void on_btnGestionGlobale_cliked();
	void on_btnManuel_cliked();
	void on_btnOuvrirBarriere_clicked();

	void resetInterface();

	void on_btnDeconnexion_clicked();

	// void onClientConnected();
	// void onClientReadyRead();
	// void sendLicensePlateRequest();

private:
	Plate_Management* plateManagement;
	Ui::Process_BarriereClass ui;
	DatabaseConnect databaseConnect;
	QTcpServer* server; // Déclaration du serveur TCP/IP
	QTcpSocket* clientConnection; // Déclaration socket pour la connexion avec les clients

	QMap<QTcpSocket*, Clients*> clients; // Liste QMap pour les clients  
	QMap<QString, QString> clientsConnus; // Liste des clients du projet

};
