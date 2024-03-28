#pragma once
#include <QObject>

class ManageMode : public QObject
{
    Q_OBJECT

public:
    ManageMode(QObject* parent = nullptr);
    ~ManageMode();

public slots:
    void Mode_CasParCas();
    void Mode_GestionGlobale();
    void Mode_Manuel();
    // Autres méthodes pour gérer les différents modes

private:
    // Méthodes privées pour la logique spécifique à chaque mode
};
