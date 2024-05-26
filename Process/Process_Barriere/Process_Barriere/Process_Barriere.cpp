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

	// Param�tres de visibilit� graphiques
	ui.stackedWidget->setCurrentIndex(0);
	ui.widget_SCStatut->setVisible(false);
	ui.edit_Mdp->setEchoMode(QLineEdit::Password);

	
	plateManagement = new Plate_Management();

	QObject::connect(ui.btn_OuvertureBarriere, SIGNAL(clicked()), this, SLOT(on_btnOuvrirBarriere_clicked()));
	modeActif = Manuel; // Mode par d�faut
}

Process_Barriere::~Process_Barriere()
{
	delete server;
}


// ==== PARTIE SERVEUR ====

void Process_Barriere::onClientConnected()
{
	/* onClientConnected() : �tablit une connexion avec le client. */

	clientConnection = server->nextPendingConnection();
	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
}


void Process_Barriere::onClientReadyRead()
{
	/* onClientReadyRead() : permet de lire les donn�es envoy�e par le client.
	Se d�compose en 2 = une partie pour le signal v�hicule d�tect�, une partie pour la r�ception de la plaque. */

	QByteArray data = clientConnection->readAll();
	QString str(data);
	qDebug() << "Message recu du client :" << str;

	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // D�codage JSON

	// �tape 1
	if (jsonMessage.contains("InfoVeh") && jsonMessage["InfoVeh"].toString() == "VehiculeDetecter")
	{
		qDebug() << "Vehicule detecte par le client.";
		ui.label_VehiculePresenceDisplay->setText("Vehicule detecte");
		sendLicensePlateRequest();
		return;
	}

	// �tape 2 :
	if (jsonMessage.contains("reponsePlaqueReco")) {
		plaque = jsonMessage["reponsePlaqueReco"].toString(); // Affectation de la valeur de plaque
		qDebug() << "Plaque recue :" << plaque;

		ui.label_StatutClientDisplay->setText("Plaque recue.");
		ui.label_ImmatriculationDisplay->setText(plaque);
		qDebug() << "=== FIN INTERRACTION CLIENT ===\n";
		plateManagement->AnalysePlaque(plaque, modeActif, ui);
	}
}

void Process_Barriere::sendLicensePlateRequest()
{
	/* sendLicensePlateRequest() : envoi au client une demande de r�cup�ration de plaque. */

	if (clientConnection) {
		qDebug() << "Envoi de la demande de reconnaissance de plaque.";

		QJsonObject message;
		QJsonArray tableauDonnees;

		message["DemandeRecoPlaque"] = "DemandeOUI";

		// Conversion de l'objet JSON en cha�ne JSON
		QJsonDocument jsonDocument(message);
		QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

		clientConnection->write(jsonString.toUtf8());

		ui.label_StatutServeurDisplay->setText("Demande envoyee");
	}
}


// ==== PARTIE Accueil ====

void Process_Barriere::on_btnAccesConnexion_clicked()
{
	/* on_btnAccesConnexion_clicked() : v�rifie en BDD si l'utilisateur a les permissions n�cessaires pour 
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
	// TODO : Appeler les m�thodes dans plate management.
	qDebug() << "Slot btnOuvrirBarriere_clicked";
}
