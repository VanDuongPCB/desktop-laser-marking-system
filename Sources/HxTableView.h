#pragma once
#include <QTableView>
#include "HxDataTable.h"

class HxTableView : public QTableView
{
public:
    HxTableView( QWidget* parent = nullptr );
    ~HxTableView();
    QStringList headers;
    HxDataTable* DataTable();
    void SetHeaders( QStringList headers );
    void SetRowCount( int count );
    QStandardItem* Item( int row, int col );
    QStandardItem* Item( int row, QString header );
    void SetText( int row, int col, QString data );
    void SetText( int row, QString header, QString data );
protected:
    void keyPressEvent( QKeyEvent* event );
};
