#include "addparamsdialog.h"
#include "ui_addparamsdialog.h"

AddParamsDialog::AddParamsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddParamsDialog){
    ui->setupUi(this);
}

AddParamsDialog::~AddParamsDialog(){
    delete ui;
}

void AddParamsDialog::on_btnAdd_clicked(){
    QString text = ui->txtParamNames->toPlainText().trimmed().toUpper();
    QStringList items = text.split(',');
    names.clear();
    for(auto &it : items){
        QString item = it.trimmed();
        if(item.length()<=0) continue;
        if(names.contains(item)) continue;
        names.push_back(item);
    }

    if(names.size()>0){
        isApplyAll = ui->chxApplyAll->isChecked();
        isDefault = ui->chxDefault->isChecked();
        this->close();
        this->setResult(1);
    }
}

