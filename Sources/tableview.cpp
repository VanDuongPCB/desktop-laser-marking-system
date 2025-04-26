#include "tableview.h"
#include <QKeyEvent>
#include <iostream>

TableView::TableView(QWidget *parent) : QTableView(parent){

}

TableView::~TableView(){

}

void TableView::keyPressEvent(QKeyEvent *event){
    QTableView::keyPressEvent(event);
    int key = event->key();
    // up + down
    if(key == 16777235 || key == 16777237){
        emit pressed(this->currentIndex());
    }
}

DataTable *TableView::dataTable(){
     DataTable* m = (DataTable*)this->model();
     if(m == nullptr){
         m = new DataTable();
         this->setModel(m);
     }
     return m;
}

void TableView::setHeaders(QStringList headers){
    this->headers = headers;
    DataTable *m = dataTable();
    m->setColumnCount(headers.size());
    m->setHorizontalHeaderLabels(headers);
}

void TableView::setRowCount(int count){
    dataTable()->setRowCount(count);
}

QStandardItem *TableView::item(int row, int col){
    return ((QStandardItemModel*)model())->item(row, col);
}

QStandardItem *TableView::item(int row, QString header){
    for(int i=0;i<headers.size();i++){
        if(header == headers[i]){
            return ((QStandardItemModel*)model())->item(row, i);
        }
    }
    return nullptr;
}

void TableView::setText(int row, int col, QString data){
    DataTable *m = dataTable();
    if(row < 0 || row >= m->rowCount()) return;
    if(col < 0 || col >= m->columnCount()) return;
    QStandardItem * item = (QStandardItem*)m->item(row, col);
    if(item == nullptr){
        item = new QStandardItem();
        m->setItem(row, col, item);
    }
    item->setText(data);
}

void TableView::setText(int row, QString header, QString data){
    DataTable *m = dataTable();
    if(row < 0 || row >= m->rowCount()) return;
    for(int col = 0; col < headers.size(); col++){
        if(headers[col] == header){
            setText(row, col, data);
            break;
        }
    }
}


