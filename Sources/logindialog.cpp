#include "logindialog.h"
#include "ui_logindialog.h"
#include "protector.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog){
    ui->setupUi(this);
}

LoginDialog::~LoginDialog(){
    delete ui;
}

bool LoginDialog::checkInputs(){
    QString user = ui->txtUserName->text().trimmed().toUpper();
    QString pass = ui->txtPassword->text().trimmed();
    return user.length() > 0 && pass.length() > 0;
}

void LoginDialog::on_txtUserName_textChanged(const QString &arg1){
    ui->btnLogin->setEnabled(checkInputs());
}

void LoginDialog::on_txtPassword_textChanged(const QString &arg1){
    ui->btnLogin->setEnabled(checkInputs());
}

void LoginDialog::on_btnLogin_clicked(){
    QString user = ui->txtUserName->text().trimmed().toUpper();
    QString pass = ui->txtPassword->text().trimmed();
    if(Protector::instance()->login(user, pass)){
        this->close();
        this->setResult(1);
    }
}
