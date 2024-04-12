#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRandomGenerator>

class ClientSimulation : public QObject
{
    Q_OBJECT

public:
    ClientSimulation(QObject* parent = nullptr);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void sendVehicleDetection();

private:
    QTcpSocket* socketClient;
    QString getRandomPlate();
};


