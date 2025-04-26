#ifndef DATATABLE_H
#define DATATABLE_H

#include <QTableView>
#include <QStandardItemModel>
class DataTable : public QStandardItemModel{
public:
    DataTable();
    DataTable(int rows, int cols);
    ~DataTable();


public:
    static DataTable* fromTableView(QTableView *table, QStringList headers);
};

#endif // DATATABLE_H
