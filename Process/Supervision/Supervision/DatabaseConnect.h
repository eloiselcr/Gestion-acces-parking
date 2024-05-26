#pragma once
#include <QtSql>

class DatabaseConnect
{
public:
    DatabaseConnect();

private:
    QSqlDatabase db;
};