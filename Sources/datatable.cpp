#include "datatable.h"

DataTable::DataTable() : QStandardItemModel(nullptr){

}

DataTable::DataTable(int rows, int cols) : QStandardItemModel(rows, cols, nullptr){

}

DataTable::~DataTable(){

}

