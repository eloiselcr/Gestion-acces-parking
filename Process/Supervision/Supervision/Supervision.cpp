#include "Supervision.h"

Supervision::Supervision(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

	// Init du serveur
	server = new QTcpServer(this);
	connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
	server->listen(QHostAddress::Any, 1234);

	// Init des adresses IP des équipements
	clientsConnus["::ffff:192.168.65.33"] = "Arduino1";
	clientsConnus["::ffff:192.168.65.95"] = "Arduino2";
	clientsConnus["::ffff:192.168.64.91"] = "PCQuentin";

	// Paramètres de visibilité graphiques
	ui.stackedWidget->setCurrentIndex(0);
	ui.edit_Mdp->setEchoMode(QLineEdit::Password);
}

Supervision::~Supervision()
{
	delete server;
}


// ==== PARTIE Accueil ====

void Supervision::on_btnAccesConnexion_clicked()
{
	/* on_btnAccesConnexion_clicked() : vérifie en BDD si l'utilisateur a les permissions
	nécessaires pour se connecter, ainsi que les bons identifiants. */

	QString login = ui.edit_Login->text();
	QString mdp = ui.edit_Mdp->text();

	QSqlQuery query;
	query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
	query.bindValue(":login", login);
	query.bindValue(":mdp", mdp);
	query.exec();

	qDebug() << "Requete SQL efffectuee";

	if (query.next()) {
		ui.label_ErrorConnect->setText("Identifiants corrects !");
		ui.stackedWidget->setCurrentIndex(1);
		ui.label_BienvenueTexte->setVisible(false);

		qDebug() << "ID corrects";
	}
	else {
		ui.label_ErrorConnect->setText("Identifiants incorrects");

		qDebug() << "ID incorrects";
	}
}


// ==== PARTIE Supervision ====


void Supervision::onClientConnected()
{
	/* onClientConnected() : permet au client de se connecter et de vérifier si l'IP est connue. 
	Envoi une demande d'état des équipements. */

	QTcpSocket* clientSocket = server->nextPendingConnection();
	QString clientIp = clientSocket->peerAddress().toString();
	QString clientName = clientsConnus.value(clientIp, "Inconnu");

	Clients* newClient = new Clients(clientSocket, clientName);
	clients[clientSocket] = newClient;

	connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
	connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

	qDebug() << "Client connecté : " << clientName << " avec l'IP : " << clientIp;

	sendEtatCamera();
	sendEtatArd1();
	sendEtatArd2();
}


void Supervision::onClientReadyRead()
{
	/* onClientReadyRead() : permet de lire les données reçues et de les convertir en JSON. */

	QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
	Clients* client = clients.value(senderSocket, nullptr);

	if (!client) {
		qDebug() << "Client non trouvé pour le socket : " << senderSocket;
		return;
	}

	QByteArray data = senderSocket->readAll();
	QString str(data);
	qDebug() << "Message reçu du client :" << client->getName() << " : " << str;

	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object();
	interractClient(client, jsonMessage);
}


void Supervision::sendEtatCamera()
{
	/* sendEtatCamera() : envoi une demande pour récupérer l'état de la caméra. */

	if (!clients.isEmpty()) {
		for (Clients* client : clients) {
			qDebug() << "Vérification du client pour l'envoi de la demande d'état : " << client->getName();
			if (client->getName() == "PCQuentin") {

				qDebug() << "Envoi de la demande d'état de la caméra à :  " << client->getName();

				QJsonObject messageCam;
				messageCam["DemandeEtatCamera"] = "DemandeEtat";

				QJsonDocument jsonDocument(messageCam);
				QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

				client->getSocket()->write(jsonString.toUtf8());
			}
		}
	}
}


void Supervision::sendEtatArd1()
{
	/* sendEtatArd1() : envoi une demande pour récupérer l'état de l'Arduino 1. */

	if (!clients.isEmpty()) {
		for (Clients* client : clients) {
			qDebug() << "Vérification du client pour l'envoi de la demande d'état : " << client->getName();
			if (client->getName() == "PCQuentin") {

				qDebug() << "Envoi de la demande d'état de l'arduino 1 à :  " << client->getName();

				QJsonObject messageArd1;
				messageArd1["DemandeEtatArd1"] = "DemandeEtat";

				QJsonDocument jsonDocument(messageArd1);
				QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

				client->getSocket()->write(jsonString.toUtf8());
			}
		}
	}
}


void Supervision::sendEtatArd2()
{
	/* sendEtatArd2() : envoi une demande pour récupérer l'état de l'Arduino 2. */

	if (!clients.isEmpty()) {
		for (Clients* client : clients) {
			qDebug() << "Vérification du client pour l'envoi de la demande d'état : " << client->getName();
			if (client->getName() == "PCQuentin") {

				qDebug() << "Envoi de la demande d'état de la caméra à :  " << client->getName();

				QJsonObject messageArd2;
				messageArd2["DemandeEtatArd2"] = "DemandeEtat";

				QJsonDocument jsonDocument(messageArd2);
				QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

				client->getSocket()->write(jsonString.toUtf8());
			}
		}
	}
}


void Supervision::interractClient(Clients* client, const QJsonObject& jsonMessage)
{
	/* interractClient() : permet de lire les données envoyée par le client.
	Se décompose pour les 3 équipements : la caméra, l'arduino 1 et l'arduino 2. */

	// Caméra
	if (jsonMessage.contains("EtatCamera") && jsonMessage["EtatCamera"].toString() == "HorsLigne") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutcamera->setText("Hors Ligne");
		return;
	}
	if (jsonMessage.contains("EtatCamera") && jsonMessage["EtatCamera"].toString() == "Allumee") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutcamera->setText("En ligne");
		return;
	}

	// Arduino 1 
	if (jsonMessage.contains("EtatArd1") && jsonMessage["EtatArd1"].toString() == "HorsLigne") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutard1->setText("Hors Ligne");
		return;
	}
	if (jsonMessage.contains("EtatArd1") && jsonMessage["EtatArd1"].toString() == "Allumee") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutard1->setText("En ligne");
		return;
	}

	// Arduino 2
	if (jsonMessage.contains("EtatArd2") && jsonMessage["EtatArd2"].toString() == "HorsLigne") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutard2->setText("Hors Ligne");
		return;
	}
	if (jsonMessage.contains("EtatArd2") && jsonMessage["EtatArd2"].toString() == "Allumee") {
		qDebug() << "Message de :  " << client->getName();
		ui.label_statutard2->setText("En ligne");
		return;
	}
}
