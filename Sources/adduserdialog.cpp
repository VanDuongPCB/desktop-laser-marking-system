#include "adduserdialog.h"
#include "ui_adduserdialog.h"
#include "user.h"

AddUserDialog::AddUserDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddUserDialog){
    ui->setupUi(this);
}

AddUserDialog::~AddUserDialog(){
    delete ui;
}

void AddUserDialog::on_btnAdd_clicked(){
    QString user = ui->txtUser->text().trimmed();
    QString userLower = user.toLower();
    for(auto &it : User::items){
        if(it->name.toLower() == userLower){
            ui->lblError->setText("Đã có user này !");
            return;
        }
    }
    User *us = new User();
    us->name = user;

    User::items.emplace_back(us);
    this->close();
    this->setResult(1);
}


void AddUserDialog::on_txtUser_textChanged(const QString &arg1){
    ui->btnAdd->setEnabled(false);
    QString user = arg1.trimmed().toLower();
    if(user.length()<1){

        return;
    }
    for(auto &usr : User::items){
        if(usr->name == user){
            return;
        }
    }
    ui->btnAdd->setEnabled(true);
}

