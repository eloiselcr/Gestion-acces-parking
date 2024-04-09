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

    plaque = "AA-508-CP";


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


void Process_Barriere::onClientReadyRead()
{
    QByteArray data = clientConnection->readAll();
    QString str(data);
    qDebug() << "Message recu du client :" << str;

    QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // Décodage JSON

    // Etape 1 : Le vehicule a ete detecte
    if (jsonMessage.contains("InfoVeh") && jsonMessage["InfoVeh"].toString() == "VehiculeDetecter")
    {
        ui.label_VehiculePresenceDisplay->setText("Vehicule detecte");
        sendLicensePlateRequest();
        // return; 
    }

    // Etape 2 : Reception de la plaque
    if (jsonMessage.contains("reponsePlaqueReco")) {
        plaque = jsonMessage["reponsePlaqueReco"].toString(); // Affectation de la valeur de plaque
        qDebug() << "Plaque recue :" << plaque;

        ui.label_StatutClientDisplay->setText("Plaque recue");
        ui.label_ImmatriculationDisplay->setText(plaque);
    }
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

    qDebug() << "Requete SQL efffectuee";

    if (query.next()) { // Vérification des identifiants
        ui.label_ErrorConnect->setText("Identifiants corrects !");
        ui.stackedWidget->setCurrentIndex(1);
        ui.widget_SCStatut->setVisible(true);
        ui.widget_SupervisionBarriere->setVisible(false);

        qDebug() << "ID corrects";
    }
    else {
        ui.label_ErrorConnect->setText("Identifiants incorrects");

        qDebug() << "ID incorrects";
    }
}



// === PARTIE SELECTION DES MODES ====

void Process_Barriere::on_btnAccueilGestionGlobale_cliked()
{
    ui.stackedWidget->setCurrentIndex(2); 
    // NOTE : FAIRE EN SORTE QU'IL NE PUISSE PAS ACCEDER AUX AUTRES MODES
}


void Process_Barriere::on_btnCasparCas_cliked()
{
    qDebug() << "Slots on_btnCasparCas_cliked() appele";

    QSqlQuery query;
    query.prepare("SELECT statut, date FROM Demande_Vehicule WHERE immatriculation = :plaque");
    query.bindValue(":plaque", plaque);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'execution de la requete SQL:" << query.lastError().text();
        return; 
    }

    if (query.next()) {
        QString statut = query.value(0).toString(); // récupère le statut
        QDateTime date = query.value(1).toDateTime(); // récupère la date

        qDebug() << "Plaque : " << plaque;
        qDebug() << "Statut du vehicule:" << statut;
        qDebug() << "Date de validite:" << date.toString();
        ui.label_ImmatriculationDisplay->setText(plaque);
        this->statut = statut;

        // 1 - On vérifie le statut du véhicule pour une anomalie
        if (statut == "Refusee" || statut == "Traitement en cour" || statut == "Informations demandees") {
            qDebug() << "Statut anormal : " << statut;

            if (statut == "Refusee") {
                qDebug() << "Refusee";
                ui.label_StatutVehiculeDisplay->setText("Le vehicule a ete refuse par l'administration.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
            else if (statut == "Traitement en cour") {
                qDebug() << "Traitement en cours";
                ui.label_StatutVehiculeDisplay->setText("La demande pour ce vehicule est en cours de traitement.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
            else if (statut == "Informations demandees") {
                qDebug() << "Informations demandees";
                ui.label_StatutVehiculeDisplay->setText("Des informations supplementaires ont ete demandes pour ce vehicule.");
                ui.widget_SupervisionBarriere->setVisible(true);
            }
        }

        // 2 - On vérifie si l'autorisation est <= 1 an de validité
        /*
        // NE MARCHE PAS, A REFAIRE
        else if (statut == "Valide") {
            if (date <= QDateTime::currentDateTime()) {
                qDebug() << "Validité expirée";
                ui.label_StatutVehiculeDisplay->setText("Validite expiree");
            }
            // Si tout OK, on accepte
            else {
                qDebug() << "Véhicule autorisé";
                ui.label_StatutVehiculeDisplay->setText("Vehicule autorise");
            }
        }
        */

    }
    else {
        qDebug() << "Plaque inconnue";
        ui.label_ImmatriculationDisplay->setText("Vehicule inconnu de la base de donnees.");
        ui.widget_SupervisionBarriere->setVisible(true);
    }
}

void Process_Barriere::on_btnOuvrirBarriere_clicked() {
    // Déterminer le motif de problème en fonction du statut
    QString motif;
    qDebug() << "Entree dans le mode Supervision";
    qDebug() << "Statut du vehicule:" << statut;


    if (statut == "Refusee") {
        motif = "Vehicule refuse par l'administration";
    }
    else if (statut == "Traitement en cour") {
        motif = "Demande en cours de traitement";
    }
    else if (statut == "Informations demande") {
        motif = "Informations supplementaires demandees";
    }
    else if (statut == "Plaque inconnue") {
        motif = "Plaque inconnue dans la base de donnees";
    }

    // Envoyer la requête d'insertion dans la table "Acces_SansDemande"
    QSqlQuery query;
    query.prepare("INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)");
    query.bindValue(":date", QDateTime::currentDateTime());
    query.bindValue(":immatriculation", plaque);
    query.bindValue(":motif", motif);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'insertion dans la table Acces_SansDemande:" << query.lastError().text();
    }
    else {
        qDebug() << "Insertion dans la table Acces_SansDemande reussie.";
    }
}


