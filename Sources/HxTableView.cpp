#include "HxTableView.h"
#include <QKeyEvent>


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

HxDataTable* HxTableView::DataTable()
{
    HxDataTable* m = ( HxDataTable* )this->model();
    if ( m == nullptr )
    {
        m = new HxDataTable();
        this->setModel( m );
    }
    return m;
}

void HxTableView::SetHeaders( QStringList headers )
{
    this->headers = headers;
    HxDataTable* m = DataTable();
    m->setColumnCount( headers.size() );
    m->setHorizontalHeaderLabels( headers );
}

void HxTableView::SetRowCount( int count )
{
    DataTable()->setRowCount( count );
}

QStandardItem* HxTableView::Item( int row, int col )
{
    return ( ( QStandardItemModel* )model() )->item( row, col );
}

QStandardItem* HxTableView::Item( int row, QString header )
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

void HxTableView::SetText( int row, int col, QString data )
{
    HxDataTable* m = DataTable();
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

void HxTableView::SetText( int row, QString header, QString data )
{
    HxDataTable* m = DataTable();
    if ( row < 0 || row >= m->rowCount() ) return;
    for ( int col = 0; col < headers.size(); col++ )
    {
        if ( headers[ col ] == header )
        {
            SetText( row, col, data );
            break;
        }
    }
}


