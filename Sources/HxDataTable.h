#pragma once
#include "QTableView"
#include "QStandardItemModel"
class HxDataTable : public QStandardItemModel
{
public:
    HxDataTable();
    HxDataTable( int rows, int cols );
    ~HxDataTable();


public:
    static HxDataTable* FromTableView( QTableView* table, QStringList headers );
};

