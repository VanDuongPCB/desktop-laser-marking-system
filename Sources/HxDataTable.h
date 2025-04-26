#pragma once
#include <QTableView>
#include <QStandardItemModel>
class HxDataTable : public QStandardItemModel
{
public:
    HxDataTable();
    HxDataTable( int rows, int cols );
    ~HxDataTable();


public:
    static HxDataTable* fromTableView( QTableView* table, QStringList headers );
};

