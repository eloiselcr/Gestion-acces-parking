#include <qtimer.h>
#include "ClientSimulation.h"

ClientSimulation::ClientSimulation(QObject* parent) : QObject(parent)
{
    qDebug() << "=== Programme pour simuler l'envoi d'une plaque par le client. ===\n\n";

    socketClient = new QTcpSocket(this);

    connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));

    qDebug() << "Tentavie de connexion au serveur";
    socketClient->connectToHost("127.0.0.1", 1234);
   
}

void ClientSimulation::onSocketConnected()
{
    if (socketClient->state() == QTcpSocket::ConnectedState)
    {
        qDebug() << "Serveur connecte !\n";
        QTimer::singleShot(5000, this, SLOT(sendVehicleDetection())); // 5000 ms = 5 secondes
        //sendVehicleDetection();
    }
    else
        qDebug() << "Serveur introuvable\n";
}

void ClientSimulation::onSocketDisconnected()
{
    qDebug() << "Serveur deconnecte !\n";
}

void ClientSimulation::sendVehicleDetection()
{
    QJsonObject message;
    message["InfoVeh"] = "VehiculeDetecter";

    QJsonDocument jsonDocument(message);
    QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

    socketClient->write(jsonString.toUtf8());

    qDebug() << "Message de detection de vehicule envoye.\n";
}

void ClientSimulation::onSocketReadyRead()
{
    QByteArray data = socketClient->read(socketClient->bytesAvailable());
    QString str(data);

    qDebug() << "Message recu du serveur : " << str;

    QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // On décode en objet JSON
    if (jsonMessage.contains("DemandeRecoPlaque") && jsonMessage["DemandeRecoPlaque"].toString() == "DemandeOUI")
    {
        QJsonObject reponsePlaque;
        QJsonArray tableauDonnees;

        qDebug() << "Requete de reconnaisance de plaque recu. Repondre au serveur";

        LicensePlate = "JQ-657-ML";

        reponsePlaque["reponsePlaqueReco"] = LicensePlate;

        QJsonDocument jsonDocument(reponsePlaque);
        QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

        if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
            socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur

        qDebug() << "\nMessage retourne au serveur !";
    }
}


