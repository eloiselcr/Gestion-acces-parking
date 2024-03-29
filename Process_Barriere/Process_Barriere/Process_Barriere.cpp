#include "Process_Barriere.h"

Process_Barriere::Process_Barriere(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Init du serveur
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
    server->listen(QHostAddress::Any, 1234);

    // Paramètres de visibilité graphiques
    ui.stackedWidget->setCurrentIndex(0);
    ui.widget_SCStatut->setVisible(false);
    ui.edit_Mdp->setEchoMode(QLineEdit::Password);

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


// ==== PARTIE SERVEUR ====


void Process_Barriere::onClientConnected()
{
    clientConnection = server->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
}

void Process_Barriere::sendLicensePlateRequest()
{
    if (clientConnection) {
        QJsonObject message;
        QJsonArray tableauDonnees;

        message["DemandeRecoPlaque"] = "DemandeOUI";

        // Conversion de l'objet JSON en chaîne JSON
        QJsonDocument jsonDocument(message);
        QString jsonString = jsonDocument.toJson(QJsonDocument::Compact); 

        // if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
        clientConnection->write(jsonString.toUtf8());

        ui.label_StatutServeurDisplay->setText("Demande envoyee");
    }
}

void Process_Barriere::onClientReadyRead()
{
    QByteArray data = clientConnection->readAll();
    QString str(data);
    qDebug() << "Message recu du client :" << str;

    // Décodage JSON de la présence d'un véhicule
    QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object();

    if (jsonMessage.contains("InfoVeh") && jsonMessage["InfoVeh"].toString() == "VehiculeDetecter") 
    {
        ui.label_VehiculePresenceDisplay->setText("Vehicule detecte");
        sendLicensePlateRequest(); 
        // return; 
    }

    if (jsonMessage.contains("reponsePlaqueReco")) {
        QString plaque = jsonMessage["reponsePlaqueReco"].toString();
        qDebug() << "Plaque recue :" << plaque;

        ui.label_StatutClientDisplay->setText("Plaque recue");
        ui.label_ImmatriculationDisplay->setText(plaque);
    }
}




// ==== PARTIE Accueil ====

void Process_Barriere::on_btnAccesConnexion_clicked() // Form Connexion
{
    QString login = ui.edit_Login->text();
    QString mdp = ui.edit_Mdp->text();
    
    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE login = :login AND password = :mdp AND type = 'superviseur'");
    query.bindValue(":login", login);
    query.bindValue(":mdp", mdp);
    query.exec();

    if (query.next()) { // Vérification des identifiants
        ui.label_ErrorConnect->setText("Identifiants corrects !");
        ui.stackedWidget->setCurrentIndex(1);
        ui.widget_SCStatut->setVisible(true);
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




