#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

class ClientSimulation : public QObject
{
    Q_OBJECT

public:
    ClientSimulation(QObject* parent = nullptr);
    QString LicensePlate;

private slots:

    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void sendVehicleDetection();

private:
    QTcpSocket* socketClient;
};


