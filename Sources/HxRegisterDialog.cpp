#include "HxRegisterDialog.h"
#include "ui_hxregisterdialog.h"
#include "HxLicense.h"
#include "HxMessage.h"

HxRegisterDialog::HxRegisterDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::RegisterDialog )
{
    ui->setupUi( this );
    ui->txtID->setText( Licensing()->ID() );
    ui->txtVersion->setText(Licensing()->GetVersion());
    ui->txtKey->setFocus();
    connect(ui->btnRegister, &QPushButton::clicked, this, &HxRegisterDialog::OnRegister);
    connect(ui->txtKey, &QLineEdit::returnPressed, this, &HxRegisterDialog::OnMakeKeyOrRegister);
}

HxRegisterDialog::~HxRegisterDialog()
{
    delete ui;
}

void HxRegisterDialog::OnRegister()
{
    QString keyIn = ui->txtKey->text().trimmed();
    if ( Licensing()->RegisterKey( keyIn ) )
    {
        this->close();
        this->setResult( 1 );
    }
    else
    {
        HxMsgError(tr("Key không hợp lệ!"));
    }
}


void HxRegisterDialog::OnMakeKeyOrRegister()
{
    QString txt = ui->txtKey->text().trimmed().toUpper();
    if ( txt.startsWith( "--KEYGEN=" ) )
    {
        txt = txt.replace( "--KEYGEN=", "" );
        ui->txtKey->setText( Licensing()->KeyFromId( txt ) );
    }
}

