#include "Querys.h"

QString SQLQueries::getqueryInfos(const QString& plaque) {
    return "SELECT id, nom, prenom, statut, date, iduser FROM Demande_Vehicule WHERE immatriculation = :plaque";
}

QString SQLQueries::insertAcces() {
    return "INSERT INTO Acces (date_horaire, id_demande) VALUES (:date, :id_demande)";
}

QString SQLQueries::insertAccesSD() {
    return "INSERT INTO Acces_SansDemande (date_horaire, immatriculation, motif) VALUES (:date, :immatriculation, :motif)";
}
