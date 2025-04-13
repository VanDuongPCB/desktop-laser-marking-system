#include "HxTableView.h"
#include "QKeyEvent"
#include "iostream"

HxTableView::HxTableView( QWidget* parent ) : QTableView( parent )
{

}

HxTableView::~HxTableView()
{

}

void HxTableView::keyPressEvent( QKeyEvent* event )
{
    QTableView::keyPressEvent( event );
    int key = event->key();
    // up + down
    if ( key == 16777235 || key == 16777237 )
    {
        emit pressed( this->currentIndex() );
    }
}

HxDataTable* HxTableView::dataTable()
{
    HxDataTable* m = ( HxDataTable* )this->model();
    if ( m == nullptr )
    {
        m = new HxDataTable();
        this->setModel( m );
    }
    return m;
}

void HxTableView::setHeaders( QStringList headers )
{
    this->headers = headers;
    HxDataTable* m = dataTable();
    m->setColumnCount( headers.size() );
    m->setHorizontalHeaderLabels( headers );
}

void HxTableView::setRowCount( int count )
{
    dataTable()->setRowCount( count );
}

int HxTableView::RowCount()
{
    return dataTable()->rowCount();
}

QStandardItem* HxTableView::item( int row, int col )
{
    return ( ( QStandardItemModel* )model() )->item( row, col );
}

QStandardItem* HxTableView::item( int row, QString header )
{
    for ( int i = 0; i < headers.size(); i++ )
    {
        if ( header == headers[ i ] )
        {
            return ( ( QStandardItemModel* )model() )->item( row, i );
        }
    }
    return nullptr;
}

void HxTableView::setText( int row, int col, QString data )
{
    HxDataTable* m = dataTable();
    if ( row < 0 || row >= m->rowCount() ) return;
    if ( col < 0 || col >= m->columnCount() ) return;
    QStandardItem* item = ( QStandardItem* )m->item( row, col );
    if ( item == nullptr )
    {
        item = new QStandardItem();
        m->setItem( row, col, item );
    }
    item->setText( data );
}

void HxTableView::setText( int row, QString header, QString data )
{
    HxDataTable* m = dataTable();
    if ( row < 0 || row >= m->rowCount() ) return;
    for ( int col = 0; col < headers.size(); col++ )
    {
        if ( headers[ col ] == header )
        {
            setText( row, col, data );
            break;
        }
    }
}


