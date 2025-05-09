#include "HxDatabase.h"


HxDatabase::HxDatabase() : QSqlDatabase() 
{}

HxDatabase::HxDatabase( const QSqlDatabase& other ): QSqlDatabase(other)
{ }

HxDatabase::HxDatabase( const QString& fileDB ) : QSqlDatabase()
{
    setDatabaseName( fileDB );
}