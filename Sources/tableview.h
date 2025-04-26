#ifndef TABLEVIEW_H
#define TABLEVIEW_H
#include <QTableView>
#include "datatable.h"

class TableView : public QTableView{
public:
    TableView(QWidget *parent = nullptr);
    ~TableView();
    QStringList headers;
    DataTable *dataTable();
    void setHeaders(QStringList headers);
    void setRowCount(int count);
    QStandardItem *item(int row, int col);
    QStandardItem *item(int row, QString header);
    void setText(int row, int col, QString data);
    void setText(int row, QString header, QString data);
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // TABLEVIEW_H
