#include "removeparamdialog.h"
#include "ui_removeparamdialog.h"

RemoveParamDialog::RemoveParamDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RemoveParamDialog){
    ui->setupUi(this);
}

RemoveParamDialog::~RemoveParamDialog(){
    delete ui;
}

void RemoveParamDialog::setParams(QStringList names){
    ui->txtParamNames->setPlainText(names.join(','));
}

void RemoveParamDialog::on_btnRemove_clicked(){
    isApplyAll = ui->chxApplyAll->isChecked();
    this->close();
    this->setResult(1);
}

