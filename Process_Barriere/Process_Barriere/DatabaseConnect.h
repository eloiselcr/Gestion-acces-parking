#pragma once
#include <QtSql>

class DatabaseConnect
{

public:
    DatabaseConnect();
    bool isConnected() const;

private:
    QSqlDatabase db;

};


