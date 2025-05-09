#pragma once
#include "QSQlDatabase"
#include "QSqlQuery"
#include "QSqlError"

using HxQuery = QSqlQuery;

class HxDatabase : public QSqlDatabase
{
public:
    HxDatabase();
    HxDatabase( const QSqlDatabase& other );
    HxDatabase( const QString& fileDB );
};