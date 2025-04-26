#include "newmodeldialog.h"
#include "ui_newmodeldialog.h"
#include "model.h"


NewModelDialog::NewModelDialog(QWidget *parent) : QDialog(parent),ui(new Ui::NewModelDialog){
    ui->setupUi(this);
}

NewModelDialog::~NewModelDialog(){
    delete ui;
}

bool NewModelDialog::checkInputs(){
    QString name = ui->txtModelName->text().trimmed().toUpper();
    QString code = ui->txtModelCode->text().trimmed().toUpper();

    if(name.length()<1 || code.length() < 1) return false;
    for(auto &md : Model::items){
        if(md->name == name || md->code == code){
            return false;
        }
    }
    return true;
}

void NewModelDialog::on_btnCreate_clicked(){
    QString name = ui->txtModelName->text().trimmed().toUpper();
    QString code = ui->txtModelCode->text().trimmed().toUpper();
    QString no = ui->txtModelNo->text().trimmed().toUpper();

    data =Model::create(name, code);
    data->kNo = no;
    data->cvWidth = ui->spxCvWidth->value();
    Model::save(data);

    this->close();
    this->setResult(1);
}

void NewModelDialog::on_txtModelCode_textChanged(const QString &arg1){
    ui->btnCreate->setEnabled(checkInputs());
}


void NewModelDialog::on_txtModelName_textChanged(const QString &arg1){
    ui->btnCreate->setEnabled(checkInputs());
}

