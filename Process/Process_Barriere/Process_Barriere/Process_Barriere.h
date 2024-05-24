#pragma once
#include <QtWidgets/QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "ui_Process_Barriere.h"
#include "DatabaseConnect.h"
#include "Plate_Management.h"
#include "Mode.h"

class Process_Barriere : public QMainWindow {
	
	Q_OBJECT

public:

	Process_Barriere(QWidget* parent = nullptr);
	~Process_Barriere();

	QString plaque;
	Mode modeActif;

private slots:
	void on_btnAccesConnexion_clicked();
	// void on_btnAccueilGestionGlobale_cliked();

	void onClientConnected();
	void sendLicensePlateRequest();
	void onClientReadyRead();

	void on_btnCasparCas_cliked();
	void on_btnGestionGlobale_cliked();
	void on_btnManuel_cliked();
	void on_btnOuvrirBarriere_clicked();


private:
	Plate_Management* plateManagement;
	Ui::Process_BarriereClass ui;
	DatabaseConnect databaseConnect;
	QTcpServer* server; // Déclaration du serveur TCP/IP
	QTcpSocket* clientConnection; // Déclaration du socket pour la connexion avec le client

};
