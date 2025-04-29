#include "HxDataTable.h"

HxDataTable::HxDataTable() : QStandardItemModel( nullptr )
{

}

HxDataTable::HxDataTable( int rows, int cols ) : QStandardItemModel( rows, cols, nullptr )
{

}

HxDataTable::~HxDataTable()
{

}

