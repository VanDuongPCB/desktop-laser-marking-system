#include "designwindow.h"
#include "ui_designwindow.h"
#include "model.h"
#include "lot.h"

DesignWindow::DesignWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::DesignWindow){
    ui->setupUi(this);

}

DesignWindow::~DesignWindow(){
    delete ui;
}

void DesignWindow::showEvent(QShowEvent*){
    showDesigns("");
    showBlocks();
    showParams();
}

void DesignWindow::showDesigns(QString filter){
    designs.clear();
    filter = filter.trimmed().toUpper();
    if(ui->tbvDesign->headers.empty()){
        ui->tbvDesign->setHeaders({"Chương trình"});
    }

    for(auto &item : Design::items){
        if(filter == "" || item->name.contains(filter)){
            designs.push_back(item);
        }
    }

    int rows = designs.size();
    ui->tbvDesign->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        ui->tbvDesign->setText(row, 0, designs[row]->name);
    }
}

void DesignWindow::showSize(){
    if(design != nullptr){
        ui->spxDesignWidth->setValue(design->width);
        ui->spxDesignHeight->setValue(design->height);
    }
    else{
        ui->spxDesignWidth->setValue(2.5);
        ui->spxDesignHeight->setValue(2.5);
    }
}

void DesignWindow::showBlocks(){
    if(ui->tbvBlocks->headers.empty()){
        ui->tbvBlocks->setHeaders({"Block","Là mã ?","Độ dài","Nội dung"});
        ui->tbvBlocks->setColumnWidth(0, 80);
        ui->tbvBlocks->setColumnWidth(1, 80);
        ui->tbvBlocks->setColumnWidth(2, 80);
    }
    QStandardItemModel *model = (QStandardItemModel*)ui->tbvBlocks->model();
    disconnect(model, &QStandardItemModel::itemChanged, this, &DesignWindow::blockChanged);
    int rows = 32;
    ui->tbvBlocks->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        ui->tbvBlocks->setText(row, "Block",QString::number(row).rightJustified(3,'0'));
        ui->tbvBlocks->item(row, "Block")->setFlags(ui->tbvBlocks->item(row,0)->flags() & ~Qt::ItemIsEditable);
        ui->tbvBlocks->setText(row, "Là mã ?", "");
        ui->tbvBlocks->item(row, "Là mã ?")->setCheckable(true);
        ui->tbvBlocks->item(row, "Là mã ?")->setCheckState(Qt::Unchecked);
        ui->tbvBlocks->item(row, "Là mã ?")->setFlags(ui->tbvBlocks->item(row,1)->flags() & ~Qt::ItemIsEditable);
        ui->tbvBlocks->setText(row, "Độ dài","0");
        ui->tbvBlocks->setText(row, "Nội dung","");
    }
    if(design != nullptr){
        QList<int> nums = design->blocks.keys();
        for(auto num : nums){
            BlockInfo block = design->blocks[num];
            ui->tbvBlocks->item(num,"Là mã ?")->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked);
            ui->tbvBlocks->setText(num,"Độ dài", QString::number(block.textLen));
            ui->tbvBlocks->setText(num,"Nội dung", block.data);
        }
    }
    connect(model, &QStandardItemModel::itemChanged, this, &DesignWindow::blockChanged);
}

void DesignWindow::showParams(){
    // model
    if(ui->tbvModelParams->headers.empty()){
        ui->tbvModelParams->setHeaders({"Thông số model"});
    }
    QStringList modelParams = Model::paramNames();
    int rows = modelParams.size();
    ui->tbvModelParams->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        ui->tbvModelParams->setText(row,0, modelParams[row]);
    }


    // lot
    if(ui->tbvLotParams->headers.empty()){
        ui->tbvLotParams->setHeaders({"Thông số lot"});
    }
    QStringList lotParams = Lot::paramNames();
    rows = lotParams.size();
    ui->tbvLotParams->setRowCount(rows);
    for(int row = 0; row < rows; row++){
        ui->tbvLotParams->setText(row,0, lotParams[row]);
    }
}

void DesignWindow::blockChanged(QStandardItem *item){
    if(design == nullptr) return;
    int row = item->row();
    bool code = ui->tbvBlocks->item(row,"Là mã ?")->checkState() == Qt::Checked;
    int len = ui->tbvBlocks->item(row, "Độ dài")->text().trimmed().toInt();
    QString data = ui->tbvBlocks->item(row, "Nội dung")->text();
    if(!design->blocks.contains(row)){
        design->blocks.insert(row, BlockInfo());
    }
    design->blocks[row].isCode = code;
    design->blocks[row].textLen = len;
    design->blocks[row].data = data;
    Design::save(design);
}

void DesignWindow::on_actionSave_triggered(){
    Design::save();
}

void DesignWindow::on_actionLoad_triggered(){
    design = nullptr;
    Design::load();
    showDesigns("");
    showBlocks();
}

void DesignWindow::on_tbvDesign_pressed(const QModelIndex &index){
    int row = index.row();
    if(row<0) return;
    design = designs[row];
    showSize();
    showBlocks();
}

void DesignWindow::on_spxDesignWidth_valueChanged(double arg1){
    if(design == nullptr) return;
    design->width = arg1;
    Design::save(design);
}

void DesignWindow::on_spxDesignHeight_valueChanged(double arg1){
    if(design == nullptr) return;
    design->height = arg1;
    Design::save(design);
}

void DesignWindow::on_tbvModelParams_doubleClicked(const QModelIndex &index){
    int row = ui->tbvBlocks->currentIndex().row();
    if(row < 0) return;
    QString param = "MODEL." + ui->tbvModelParams->item(index.row(),0)->text();
    QString oldText = ui->tbvBlocks->item(row,"Nội dung")->text();
    if(oldText.length()>0) oldText += ",";
    oldText += param;
    ui->tbvBlocks->setText(row,"Nội dung",oldText);
    Design::save(design);
}


void DesignWindow::on_tbvLotParams_doubleClicked(const QModelIndex &index){
    int row = ui->tbvBlocks->currentIndex().row();
    if(row < 0) return;
    QString param = "LOT." + ui->tbvLotParams->item(index.row(),0)->text();
    QString oldText = ui->tbvBlocks->item(row,"Nội dung")->text();
    if(oldText.length()>0) oldText += ",";
    oldText += param;
    ui->tbvBlocks->setText(row,"Nội dung",oldText);
    Design::save(design);
}

