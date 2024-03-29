#include "Process_Barriere.h"

Process_Barriere::Process_Barriere(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Création du serveur TCP/IP
    server = new QTcpServer(this);

    // Connexion des signaux et des slots pour le serveur
    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));

    server->listen(QHostAddress::Any, 1234);

    // Vérification si la BDD est accessible
    /*if (DatabaseConnect.isConnected()) { // A REGLER
    ui.label_BDDVerif->setText("Non");
    }
    else {
    ui.label_BDDVerif->setText("Oui");
    }*/
}

Process_Barriere::~Process_Barriere()
{
    delete server;
}

// ==== PARTIE WEBSOCKET ====

void Process_Barriere::sendLicensePlateRequest() // Envoi de la demande de plaque
{
    if (clientConnection) {
        // Et ensuite la retouner afin de la renvoyer au superviseur/piloteur
        // Créer un nouvel objet JSON avec la structure souhaitée
        QJsonObject message;
        QJsonArray tableauDonnees;

        message["DemandeRecoPlaque"] = "DemandeOUI";

        // Conversion de l'objet JSON en chaîne JSON
        QJsonDocument jsonDocument(message);
        QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

        // if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
        clientConnection->write(jsonString.toUtf8()); // On envoie le message au serveur

        ui.label_StatutServeurDisplay->setText("Demande envoyée");
    }
}

void Process_Barriere::onClientConnected()
{
    clientConnection = server->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
}

void Process_Barriere::onClientReadyRead()
{
    QByteArray data = clientConnection->readAll();
    QString str(data);
    qDebug() << "Message recu du client :" << str;
    ui.label_StatutClientDisplay->setText("Plaque reçue");
}




// ==== PARTIE Qt ====

void Process_Barriere::on_btnAccesConnexion_clicked() // Form Connexion
{
    QString login = ui.edit_Login->text();
    QString mdp = ui.edit_Mdp->text();
    ui.edit_Mdp->setEchoMode(QLineEdit::Password);

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
    query.bindValue(":login", login);
    query.bindValue(":mdp", mdp);
    query.exec();

    if (query.next()) { // Vérification des identifians
        ui.label_ErrorConnect->setText("Identifiants corrects !");
        ui.stackedWidget->setCurrentIndex(1);
    }
    else {
        ui.label_ErrorConnect->setText("Identifiants incorrects");
    }
}

// === PARTIE SELECTION DES MODES ====

void Process_Barriere::on_BtnAccueilGestionGlobale_cliked()
{
    ui.stackedWidget->setCurrentIndex(2); 
    // NOTE : FAIRE EN SORTE QU'IL NE PUISSE PAS ACCEDER AUX AUTRES MODES
}

void Process_Barriere::on_btnCasparCas_cliked()
{
}

void Process_Barriere::on_btnEnvoiDemande_cliked()
{
    sendLicensePlateRequest();
}


