#include "Process_Barriere.h"
#include "Plate_Management.h"

Process_Barriere::Process_Barriere(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Init du serveur
	server = new QTcpServer(this);
	connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
	server->listen(QHostAddress::Any, 1234);

	knownClients["::ffff:192.168.65.33"] = "Arduino1";
	knownClients["::ffff:192.168.65.95"] = "Arduino2";
	knownClients["::ffff:192.168.64.91"] = "PCQuentin";

	// Paramètres de visibilité graphiques
	ui.stackedWidget->setCurrentIndex(0);
	ui.widget_SCStatut->setVisible(false);
	ui.edit_Mdp->setEchoMode(QLineEdit::Password);

	plateManagement = new Plate_Management();

	QObject::connect(ui.btn_OuvertureBarriere, SIGNAL(clicked()), this, SLOT(on_btnOuvrirBarriere_clicked()));
	modeActif = Manuel; // Mode par défaut
	ui.label_ActualModeDisplay->setText("Manuel");
}

Process_Barriere::~Process_Barriere()
{
	delete server;
}


// ==== PARTIE SERVEUR ====


void Process_Barriere::onClientConnected()
{
	/* onClientConnected() : permet au client de se connecter et de vérifier si l'IP est connue. */

	QTcpSocket* clientSocket = server->nextPendingConnection();
	QString clientIp = clientSocket->peerAddress().toString();
	QString clientName = knownClients.value(clientIp, "Unknown");

	Clients* newClient = new Clients(clientSocket, clientName);
	clients[clientSocket] = newClient;

	connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
	connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

	qDebug() << "Client connecté : " << clientName << " avec l'IP : " << clientIp;
}

void Process_Barriere::onClientReadyRead()
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

void Process_Barriere::sendLicensePlateRequest()
{
	/* sendLicensePlateRequest() : envoi d'une demande de reconaissance de plaque uniquement au PCQuentin. */

	if (!clients.isEmpty()) {
		for (Clients* client : clients) {
			qDebug() << "Vérification du client pour l'envoi de la demande de reconnaissance de plaque : " << client->getName();
			if (client->getName() == "PCQuentin") {
				qDebug() << "Envoi de la demande de reconnaissance de plaque à " << client->getName();

				QJsonObject message;
				message["DemandeRecoPlaque"] = "DemandeOUI";

				QJsonDocument jsonDocument(message);
				QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

				client->getSocket()->write(jsonString.toUtf8());

				ui.label_StatutServeurDisplay->setText("Demande envoyée");
				break;
			}
		}
	}
	else {
		qDebug() << "Aucun client connecté pour envoyer la demande de reconnaissance de plaque.";
	}
}

void Process_Barriere::interractClient(Clients* client, const QJsonObject& jsonMessage)
{
	/* interractClient() : permet de lire les données envoyée par le client.
	Se décompose en 3 = le signal véhicule détecté, la réception de la plaque et l'état de la barrière. */

	// Étape 1
	if (jsonMessage.contains("InfoVeh") && jsonMessage["InfoVeh"].toString() == "VehiculeDetecter") {
		qDebug() << "Véhicule détecté par le client " << client->getName();
		ui.label_VehiculePresenceDisplay->setText("Véhicule détecté");
		// sendLicensePlateRequest();
		sendOpenBarriere();
		return;
	}

	// Étape 2
	if (jsonMessage.contains("reponsePlaqueReco")) {
		plaque = jsonMessage["reponsePlaqueReco"].toString();
		qDebug() << "Plaque reçue : " << plaque << " de " << client->getName();

		ui.label_StatutClientDisplay->setText("Plaque reçue");
		ui.label_ImmatriculationDisplay->setText(plaque);
		qDebug() << "=== FIN INTERACTION CLIENT ===\n";

		plateManagement->AnalysePlaque(plaque, modeActif, ui);
	}

	// Étape 3
	if (jsonMessage.contains("statutBarriere") && jsonMessage["statutBarriere"].toString() == "Ouverte") {
		barriere = jsonMessage["statutBarriere"].toString();
		qDebug() << "Statut de la barrière : " << barriere << " de " << client->getName();
		ui.label_StatutBarriereDisplay->setText("Ouverte");
	}
	else if (jsonMessage.contains("statutBarriere") && jsonMessage["statutBarriere"].toString() == "Fermer") {
		barriere = jsonMessage["statutBarriere"].toString();
		qDebug() << "Statut de la barrière : " << barriere << " de " << client->getName();
		ui.label_StatutBarriereDisplay->setText("Fermée");
	}
}


// ==== PARTIE Accueil ====

void Process_Barriere::on_btnAccesConnexion_clicked()
{
	/* on_btnAccesConnexion_clicked() : vérifie en BDD si l'utilisateur a les permissions nécessaires pour 
	se connecter, ainsi que les bons identifiants. */

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
		ui.widget_SCStatut->setVisible(true);
		ui.widget_SupervisionBarriere->setVisible(false);
		ui.label_BienvenueTexte->setVisible(false);

		qDebug() << "ID corrects";
	}
	else {
		ui.label_ErrorConnect->setText("Identifiants incorrects");

		qDebug() << "ID incorrects";
	}
}


// === PARTIE SELECTION DES MODES ====


void Process_Barriere::on_btnCasparCas_cliked() {
	modeActif = CasparCas;
	ui.label_ActualModeDisplay->setText("Cas par Cas");
	qDebug() << "Mode Cas par Cas actif";
}

void Process_Barriere::on_btnGestionGlobale_cliked() {
	modeActif = GestionGlobale;
	ui.label_ActualModeDisplay->setText("Gestion Globale");
	qDebug() << "Mode Gestion Globale actif";
}

void Process_Barriere::on_btnManuel_cliked() {
	modeActif = Manuel;
	ui.label_ActualModeDisplay->setText("Manuel");
	qDebug() << "Mode Manuel actif";
}

void Process_Barriere::on_btnOuvrirBarriere_clicked()
{
	// TODO : Appeler les méthodes dans plate management.
	qDebug() << "Slot btnOuvrirBarriere_clicked";
}


void Process_Barriere::on_btnDeconnexion_clicked()
{
	/* on_btnDeconnexion_clicked() : permet la déconnexion de l'interface de gestion. */

	// refermer barrière
	modeActif = Manuel;
	ui.stackedWidget->setCurrentIndex(0);
	ui.widget_SCStatut->setVisible(false);
}


void Process_Barriere::sendOpenBarriere()
{
	if (!clients.isEmpty()) {
		for (Clients* client : clients) {
			qDebug() << "Vérification du client pour l'envoi de la demande d'ouverture de la barrière : " << client->getName();
			if (client->getName() == "Arduino2") {
				qDebug() << "Envoi de la demande d'ouverture de la barriere " << client->getName();

				QJsonObject message;
				message["DemandeOpenBarriere"] = "DemandeOUVERTURE";

				QJsonDocument jsonDocument(message);
				QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

				client->getSocket()->write(jsonString.toUtf8());

				ui.label_StatutServeurDisplay->setText("Demande envoyée");
				break;
			}
		}
	}
	else {
		qDebug() << "Aucun client connecté pour envoyer la demande de reconnaissance de plaque.";
	}

}

void Process_Barriere::onClientDisconnected()
{
	QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
	Clients* client = clients.value(senderSocket, nullptr);

	if (client) {
		qDebug() << "Client déconnecté : " << client->getName();
		clients.remove(senderSocket);
		delete client;
	}
}



//void Process_Barriere::onClientConnected()
//{
//	/* onClientConnected() : établit une connexion avec le client. */
//
//	clientConnection = server->nextPendingConnection();
//	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
//	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
//
//	if (clientConnection->peerAddress() == QHostAddress("::ffff:192.168.65.33"))
//		qDebug() << "Arduino Capteur/FeuBicolore connecter (192.168.65.33)";
//	else
//		qDebug() << "Un client s'est connecter : " << clientConnection->peerAddress();
//
//}
//
//
//void Process_Barriere::onClientReadyRead()
//{
//	/* onClientReadyRead() : permet de lire les données envoyée par le client.
//	Se décompose en 2 = une partie pour le signal véhicule détecté, une partie pour la réception de la plaque. */
//
//	QByteArray data = clientConnection->readAll();
//	QString str(data);
//	qDebug() << "Message recu du client :" << str;
//
//	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // Décodage JSON
//
//	// Étape 1
//	if (jsonMessage.contains("InfoVeh") && jsonMessage["InfoVeh"].toString() == "VehiculeDetecter")
//	{
//		qDebug() << "Vehicule detecte par le client.";
//		ui.label_VehiculePresenceDisplay->setText("Véhicule détecté");
//		sendLicensePlateRequest();
//		return;
//	}
//
//	// Étape 2 :
//	if (jsonMessage.contains("reponsePlaqueReco")) {
//		plaque = jsonMessage["reponsePlaqueReco"].toString(); // Affectation de la valeur de plaque
//		qDebug() << "Plaque recue :" << plaque;
//
//		ui.label_StatutClientDisplay->setText("Plaque reçue");
//		ui.label_ImmatriculationDisplay->setText(plaque);
//		qDebug() << "=== FIN INTERRACTION CLIENT ===\n";
//		plateManagement->AnalysePlaque(plaque, modeActif, ui);
//	}
//}
//
//void Process_Barriere::sendLicensePlateRequest()
//{
//	/* sendLicensePlateRequest() : envoi au client une demande de récupèration de plaque. */
//
//	if (clientConnection) {
//		qDebug() << "Envoi de la demande de reconnaissance de plaque.";
//
//		QJsonObject message;
//		QJsonArray tableauDonnees;
//
//		message["DemandeRecoPlaque"] = "DemandeOUI";
//
//		// Conversion de l'objet JSON en chaîne JSON
//		QJsonDocument jsonDocument(message);
//		QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);
//
//		clientConnection->write(jsonString.toUtf8());
//
//		ui.label_StatutServeurDisplay->setText("Demande envoyée");
//	}
//}
//

