#include "ClientSimulation.h"

ClientSimulation::ClientSimulation(QObject* parent) : QObject(parent)
{
    socketClient = new QTcpSocket(this);
    connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));

    socketClient->connectToHost("127.0.0.1", 1234);
}

void ClientSimulation::onSocketConnected()
{
    if (socketClient->state() == QTcpSocket::ConnectedState)
    {
        qDebug() << "Serveur connecte!";
        sendVehicleDetection();
    }
    else
        qDebug() << "Serveur introuvable";
}

void ClientSimulation::onSocketDisconnected()
{
    qDebug() << "Serveur deconnecte !";
}

void ClientSimulation::sendVehicleDetection()
{
    QJsonObject message;
    message["InfoVeh"] = "VehiculeDetecter";

    QJsonDocument jsonDocument(message);
    QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

    socketClient->write(jsonString.toUtf8());
}

void ClientSimulation::onSocketReadyRead()
{
    QByteArray data = socketClient->readAll();
    QString str(data);
    qDebug() << "Message recu du serveur :" << str;

    QJsonObject jsonMessage = QJsonDocument::fromJson(data).object();

    if (jsonMessage.contains("DemandeRecoPlaque") && jsonMessage["DemandeRecoPlaque"].toString() == "DemandeOUI")
    {
        // QString randomPlate = getRandomPlate();
        QString plate = "AA-508-CP";

        QJsonObject responsePlate;
        responsePlate["reponsePlaqueReco"] = plate;

        QJsonDocument jsonResponse(responsePlate);
        QString jsonString = jsonResponse.toJson(QJsonDocument::Compact);

        socketClient->write(jsonString.toUtf8());
    }
}

QString ClientSimulation::getRandomPlate()
{
    QStringList plates = { "KO-257-XC", "KK-469-JH", "MJ-800-NH" };
    int index = QRandomGenerator::global()->bounded(plates.size());
    return plates[index];
}
