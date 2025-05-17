#include "HxLoginDialog.h"
#include "ui_hxlogindialog.h"
#include "HxProtector.h"

HxLoginDialog::HxLoginDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::LoginDialog )
{
    ui->setupUi( this );
    ui->cbxProfile->setFocus();
    auto setPasswordFocus = [ & ]()
        {
            ui->txtPassword->setFocus();
        };
    connect( ui->cbxProfile, &QComboBox::currentTextChanged, this, setPasswordFocus );
    connect( ui->btnLogin, &QPushButton::clicked, this, &HxLoginDialog::OnLogin );
    setPasswordFocus();
}

HxLoginDialog::~HxLoginDialog()
{
    delete ui;
}

void HxLoginDialog::OnLogin()
{
    QString user = ui->cbxProfile->currentText().trimmed();
    QString pass = ui->txtPassword->text().trimmed();
    if ( Protector()->Login(user, pass) )
    {
        this->close();
        this->setResult( 1 );
    }
}
