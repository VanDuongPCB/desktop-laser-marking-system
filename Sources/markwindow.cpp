#include "markwindow.h"
#include "ui_markwindow.h"


#include "marker.h"
#include "message.h"
#include "exception.h"
#include "systemerror.h"
#include "laser.h"
#include "actuator.h"

#include <iostream>
#include "debugger.h"



MarkWindow::MarkWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MarkWindow){
    mainWindow = (MainWindow*)parent;
    ui->setupUi(this);
    connect(Marker::instance(), &Marker::printed, this, &MarkWindow::controllerPrinted);
    connect(Marker::instance(), &Marker::started, this, &MarkWindow::markStarted);
    connect(Marker::instance(), &Marker::stopped, this, &MarkWindow::markStopped);
    connect(SystemError::instance(), &SystemError::reported, this, &MarkWindow::handleException);

    showLots();
    showLotInfo(nullptr);
    showLotStatus(nullptr);
    showLotBlocks();
    showExceptions();
    updateUI();
}

MarkWindow::~MarkWindow()
{
    delete ui;
}

void MarkWindow::showEvent(QShowEvent*){
    showLots();
    showLotInfo(Marker::instance()->lot);
    showLotStatus(Marker::instance()->lot);
    updateUI();
}

void MarkWindow::showLots(){
    if(ui->tbvSelector->headers.empty()){
        ui->tbvSelector->setHeaders({"Tên","Model","MAC đầu","MAC cuối","Sản lượng","Tiến độ","Trạng thái"});
    }

    Lot::sort();

    int rows = (int)Lot::items.size();
    ui->tbvSelector->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        QString status = Lot::items[row]->status();

        ui->tbvSelector->setText(row,"Tên", Lot::items[row]->name);
        ui->tbvSelector->setText(row,"Model", Lot::items[row]->modelName);
        ui->tbvSelector->setText(row,"MAC đầu", Lot::items[row]->macStart);
        ui->tbvSelector->setText(row,"MAC cuối", Lot::items[row]->macEnd);
        ui->tbvSelector->setText(row,"Sản lượng", QString::number(Lot::items[row]->quantity));
        ui->tbvSelector->setText(row,"Tiến độ", QString::number(Lot::items[row]->progress) + "/" + QString::number(Lot::items[row]->quantity));
        ui->tbvSelector->setText(row,"Trạng thái", status);

        QColor background = QColor(255,255,255);
        if(status == "Đang sản xuất"){
            background = QColor(255,255,128);
        }
        else if(status == "Đã hoàn thành"){
            background = QColor(128,255,128);
        }

        for(int col = 0; col < ui->tbvSelector->dataTable()->columnCount();col++){
            ui->tbvSelector->item(row, col)->setBackground(background);
        }
    }
}

void MarkWindow::showLotInfo(std::shared_ptr<Lot> lot){
    if(lot == nullptr){
        ui->lblLotName->setText("Chưa chọn lot...");
        ui->lblSerialStart->setText("Chưa chọn lot...");
        ui->lblSerialEnd->setText("Chưa chọn lot...");
        ui->lblQuantity->setText("Chưa chọn lot...");
        ui->lblStatus->setText("Chưa chọn lot...");
        ui->lblModelName->setText("Chưa chọn lot...");
        ui->lblModelCode->setText("Chưa chọn lot...");
        ui->lblDesign->setText("Chưa chọn lot...");
        ui->lblIVProgram->setText("Chưa chọn lot...");
    }
    else{
        ui->lblLotName->setText(lot->name);
        ui->lblSerialStart->setText(lot->macStart);
        ui->lblSerialEnd->setText(lot->macEnd);
        ui->lblQuantity->setText(QString::number(lot->quantity));
        ui->lblStatus->setText(lot->status());
        ui->lblModelName->setText(lot->modelName);
        auto model = Model::find(lot->modelName);
        if(model!=nullptr){
            ui->lblModelCode->setText(model->code);
            ui->lblDesign->setText(model->design);
            ui->lblIVProgram->setText(model->ivProgram);
        }
        else{
            ui->lblModelCode->setText("Không xác định...");
            ui->lblDesign->setText("Không xác định...");
            ui->lblIVProgram->setText("Không xác định...");
        }
    }
}

void MarkWindow::showLotStatus(std::shared_ptr<Lot> lot){
    if(lot == nullptr){
        ui->lblStatus->setText("Không xác định...");
        ui->lblCount->setText("Không xác định...");
        ui->pgbProgress->setMaximum(100);
        ui->pgbProgress->setValue(0);
    }
    else{
        ui->lblStatus->setText(lot->status());
        ui->lblCount->setText(QString::number(lot->progress));
        ui->pgbProgress->setMaximum(lot->quantity);
        ui->pgbProgress->setValue(lot->progress);

        if(lot->status()=="Chưa sản xuất"){
            ui->lblStatus->setStyleSheet("background:#888");
        }
        else if(lot->status()=="Đang sản xuất"){
            ui->lblStatus->setStyleSheet("background:#ff8");
        }
        else{
            ui->lblStatus->setStyleSheet("background:#8f8");
        }
    }
}

void MarkWindow::showLotBlocks(){
    if(ui->tbvBlocks->headers.empty()){
        ui->tbvBlocks->setHeaders({"Block","Dữ liệu"});
    }
    ui->tbvBlocks->setRowCount(0);
    auto lot = Marker::instance()->lot;
    if(lot == nullptr) return;
    auto model = Marker::instance()->model;
    auto design = Marker::instance()->design;
    int codeIndex = -1;
    if(design != nullptr){
        codeIndex = design->indexOfBlockCode();
    }
    auto tempLot = std::make_shared<Lot>(Lot());
    tempLot.get()[0] = lot.get()[0];
    tempLot->progress --;
    if(tempLot->progress<0) {
        tempLot->progress = 0;
    }

    QMap<int, QString> blockdatas = BlockInfo::gen(design, tempLot, model);
    QList<int> blockNums = blockdatas.keys();
    std::sort(blockNums.begin(), blockNums.end());
    ui->tbvBlocks->setRowCount(blockNums.size());
    for(int i=0;i< blockNums.size();i++){
        ui->tbvBlocks->setText(i,"Block", QString::number(blockNums[i]).rightJustified(3,'0'));
        ui->tbvBlocks->setText(i,"Dữ liệu", blockdatas[blockNums[i]]);
        if(blockNums[i] == codeIndex){
            ui->lblBarcode->setText(blockdatas[blockNums[i]]);
        }
    }

    ui->lblFormat->setText("");
    if(codeIndex>0){
        ui->lblFormat->setText(design->blocks[codeIndex].data);
    }
}

void MarkWindow::showExceptions(){
    if(ui->tbvErrors->headers.empty()){
        ui->tbvErrors->setHeaders({"Thời gian","Từ","Lỗi"});
    }
    int rows = (int)exceptions.size();
    ui->tbvErrors->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        ui->tbvErrors->setText(row,0,exceptions[row].time);
        ui->tbvErrors->setText(row,1,exceptions[row].where);
        ui->tbvErrors->setText(row,2, exceptions[row].message.replace("\n",", "));
    }
    ui->tbvErrors->scrollToBottom();
}

void MarkWindow::updateUI(){
    auto lot = Marker::instance()->lot;
    bool hasData = lot != nullptr;
    bool canMark = hasData && lot->isCompleted() == false;
    bool busy = Marker::instance()->isBusy();

    ui->actionSelect->setEnabled(!busy);
    ui->actionLoad->setEnabled(!busy);
    ui->actionMark->setEnabled(hasData && !busy);
    ui->actionRun->setEnabled(canMark && !busy);
    ui->actionStop->setEnabled(busy);
}

void MarkWindow::handleException(Exception ex){
    exceptions.push_back(ex);
    showExceptions();
}

void MarkWindow::markStarted(){
    updateUI();
}

void MarkWindow::markStopped(){
    updateUI();
}

void MarkWindow::controllerPrinted(std::shared_ptr<Lot> lot){
    showLotStatus(lot);
    showLotBlocks();
}

void MarkWindow::on_actionSelect_triggered(){
    ui->stackedWidget->setCurrentWidget(ui->pageSelector);
    updateUI();
}

void MarkWindow::on_actionRun_triggered(){
    try{
        if(Actuator::setEnable(true)){
            Marker::instance()->start();
            ui->actionSelect->setEnabled(false);
            ui->actionLoad->setEnabled(false);
            ui->actionMark->setEnabled(false);
            ui->actionRun->setEnabled(false);
            ui->actionStop->setEnabled(false);
            if(mainWindow!=nullptr){
                mainWindow->setNavEnable(false);
            }
        }
    }
    catch(Exception ex){
        SystemError::instance()->errorReport(ex);
        ex.where = "Khắc";
//        Message::error(ex.message);
    }
}

void MarkWindow::on_actionStop_triggered(){
    try{
        if(Actuator::setEnable(false)){
            Marker::instance()->stop();
            ui->actionSelect->setEnabled(true);
            ui->actionLoad->setEnabled(false);
            ui->actionMark->setEnabled(false);
            ui->actionRun->setEnabled(false);
            ui->actionStop->setEnabled(false);
            if(mainWindow!=nullptr){
                mainWindow->setNavEnable(true);
            }
        }
    }
    catch(Exception ex){
//        Message::error(ex.message);
        ex.where = "Dừng khắc";
        SystemError::instance()->errorReport(ex);
    }

}

void MarkWindow::on_actionMark_triggered(){
    try{
        Marker::instance()->mark(true);
        Message::show("Đã khắc xong !", "In test");
    }
    catch(Exception ex){
//        Message::error(ex.message);
        ex.where = "In test";
        SystemError::instance()->errorReport(ex);
    }
}

void MarkWindow::on_tbvSelector_doubleClicked(const QModelIndex &index){
    int row = index.row();
    QString lotName = ui->tbvSelector->item(row,0)->text();
    auto lot = Lot::find(lotName);
    if(Marker::instance()->select(lot)){
        ui->stackedWidget->setCurrentWidget(ui->pageMark);
        showLotInfo(Marker::instance()->lot);
        showLotStatus(Marker::instance()->lot);
        showLotBlocks();
        updateUI();
    }
}

void MarkWindow::on_actionLoad_triggered(){
    showLots();
    showLotInfo(Marker::instance()->lot);
    showLotStatus(Marker::instance()->lot);
    showLotBlocks();
}

