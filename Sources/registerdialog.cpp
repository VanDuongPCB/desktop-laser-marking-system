#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "license.h"

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RegisterDialog){
    ui->setupUi(this);
    ui->txtID->setText(License::id());
}

RegisterDialog::~RegisterDialog(){
    delete ui;
}

void RegisterDialog::on_pushButton_clicked(){
    QString keyIn = ui->txtKey->text().trimmed();
    if(License::registerKey(keyIn)){
        this->close();
        this->setResult(1);
    }
    else{
        ui->lblError->setText("Key không hợp lệ !");
    }
}


void RegisterDialog::on_txtKey_textChanged(const QString &arg1){
    ui->lblError->setText("");
}


void RegisterDialog::on_txtKey_returnPressed(){
    QString txt = ui->txtKey->text().trimmed().toUpper();
    if(txt.startsWith("--KEYGEN=")){
        txt = txt.replace("--KEYGEN=","");
        ui->txtKey->setText(License::keyFromId(txt));
    }
}

