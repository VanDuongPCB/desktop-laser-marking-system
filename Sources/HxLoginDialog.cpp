#include "HxLoginDialog.h"
#include "ui_hxlogindialog.h"
#include "HxProtector.h"

HxLoginDialog::HxLoginDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::LoginDialog )
{
    ui->setupUi( this );
}

HxLoginDialog::~HxLoginDialog()
{
    delete ui;
}

bool HxLoginDialog::checkInputs()
{
    QString user = ui->txtUserName->text().trimmed().toUpper();
    QString pass = ui->txtPassword->text().trimmed();
    return user.length() > 0 && pass.length() > 0;
}

void HxLoginDialog::on_txtUserName_textChanged( const QString& arg1 )
{
    ui->btnLogin->setEnabled( checkInputs() );
}

void HxLoginDialog::on_txtPassword_textChanged( const QString& arg1 )
{
    ui->btnLogin->setEnabled( checkInputs() );
}

void HxLoginDialog::on_btnLogin_clicked()
{
    QString user = ui->txtUserName->text().trimmed().toUpper();
    QString pass = ui->txtPassword->text().trimmed();
    if ( HxProtector::instance()->login( user, pass ) )
    {
        this->close();
        this->setResult( 1 );
    }
}
