#include "lotwindow.h"
#include "message.h"
#include "ui_lotwindow.h"
#include "marker.h"

LotWindow::LotWindow(QWidget *parent) :QMainWindow(parent), ui(new Ui::LotWindow){
    ui->setupUi(this);
    connect(Marker::instance(), &Marker::printed, this, &LotWindow::controllerPrinted);
    showLot("");
}

LotWindow::~LotWindow(){
    delete ui;
}

void LotWindow::showEvent(QShowEvent *){
    showLot("");
}

void LotWindow::showLot(QString filter){
    filter = filter.trimmed();

    if(ui->tbvLots->headers.empty()){
        ui->tbvLots->setHeaders({"Tên","Kiểu in","Model","Sản lượng","Seri đầu","Seri cuối","Tiến độ","MAC đầu","MAC cuối","Trạng thái"});
        ui->tbvLots->setColumnWidth(0,150);
        ui->tbvLots->setColumnWidth(2,150);
        ui->tbvLots->setColumnWidth(7,150);
        ui->tbvLots->setColumnWidth(8,150);
        ui->tbvLots->setColumnWidth(9,150);
    }

    Lot::sort();

    int rows = Lot::items.size();
    ui->tbvLots->setRowCount(rows);
    for(int row = 0; row< rows; row++){
        int start = Lot::items[row]->counterStart.toInt();
        int end = start + Lot::items[row]->quantity - 1;
        QString status = Lot::items[row]->status();
        ui->tbvLots->setText(row,"Tên", Lot::items[row]->name);
        ui->tbvLots->setText(row,"Kiểu in", Lot::items[row]->isRePrint ? "In lại" : "");
        ui->tbvLots->setText(row,"Model", Lot::items[row]->modelName);
        ui->tbvLots->setText(row,"Sản lượng", QString::number(Lot::items[row]->quantity));
        ui->tbvLots->setText(row,"Seri đầu", Lot::items[row]->counterStart);
        ui->tbvLots->setText(row,"Seri cuối", QString::number(end).rightJustified(Lot::items[row]->counterStart.length(),'0'));
        ui->tbvLots->setText(row,"Tiến độ", QString::number(Lot::items[row]->progress) + "/" + QString::number(Lot::items[row]->quantity));
        ui->tbvLots->setText(row,"MAC đầu", Lot::items[row]->macStart);
        ui->tbvLots->setText(row,"MAC cuối", Lot::items[row]->macEnd);
        ui->tbvLots->setText(row,"Trạng thái", status);

        QColor background = QColor(255,255,255);
        if(status == "Đang sản xuất"){
            background = QColor(255,255,128);
        }
        else if(status == "Đã hoàn thành"){
            background = QColor(128,255,128);
        }

        for(int col = 0; col < ui->tbvLots->dataTable()->columnCount();col++){
            ui->tbvLots->item(row, col)->setBackground(background);
        }
    }
}


void LotWindow::controllerPrinted(std::shared_ptr<Lot> lot){
    showLot("");
}

void LotWindow::dataChanged(){
    showLot("");
}


void LotWindow::on_actionNew_triggered(){
    LotPropertiesDialog dialog(this);
    dialog.setData(nullptr);
    if(dialog.exec()){
        showLot("");
    }
}

void LotWindow::on_actionRemove_triggered(){
    std::vector<int> indexs;
    for(auto it : ui->tbvLots->selectionModel()->selectedRows()){
        indexs.push_back(it.row());
    }
    std::sort(indexs.begin(), indexs.end(), [](int i1, int i2){ return i1 > i2;});

    if(indexs.size()<=0) return;
    int ret = Message::warning("Xóa các lot đang chọn ?","Khoan đã");
    if(ret == QMessageBox::Yes){
        for(auto index : indexs){
            if(Lot::items[index]->status() == "Chưa sản xuất"){
                Lot::remove(index);
            }
        }
    }
    Lot::saveAll();
    showLot("");
}

void LotWindow::on_actionSave_triggered(){
    Lot::saveAll();
}

void LotWindow::on_actionLoad_triggered(){
    showLot("");
}

void LotWindow::on_tbvLots_doubleClicked(const QModelIndex &index){
    int row = index.row();
    if(row<0) return;
    QString lotName = ui->tbvLots->item(row,0)->text();
    auto lot = Lot::find(lotName);
    LotPropertiesDialog dialog(this);
    dialog.setData(lot);
    if(dialog.exec()){
        showLot("");
    }
}
