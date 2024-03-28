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
    // Autres m�thodes pour g�rer les diff�rents modes

private:
    // M�thodes priv�es pour la logique sp�cifique � chaque mode
};
