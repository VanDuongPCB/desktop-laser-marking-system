#include "HxRegisterDialog.h"
#include "ui_HxRegisterDialog.h"
#include "HxLicense.h"

HxRegisterDialog::HxRegisterDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::RegisterDialog )
{
    ui->setupUi( this );
    ui->txtID->setText( GetLicensing()->id() );
}

HxRegisterDialog::~HxRegisterDialog()
{
    delete ui;
}

void HxRegisterDialog::on_pushButton_clicked()
{
    QString keyIn = ui->txtKey->text().trimmed();
    if ( GetLicensing()->registerKey( keyIn ) )
    {
        this->close();
        this->setResult( 1 );
    }
    else
    {
        ui->lblError->setText( "Key không hợp lệ !" );
    }
}


void HxRegisterDialog::on_txtKey_textChanged( const QString& arg1 )
{
    ui->lblError->setText( "" );
}


void HxRegisterDialog::on_txtKey_returnPressed()
{
    QString txt = ui->txtKey->text().trimmed().toUpper();
    if ( txt.startsWith( "--KEYGEN=" ) )
    {
        txt = txt.replace( "--KEYGEN=", "" );
        ui->txtKey->setText( GetLicensing()->keyFromId( txt ) );
    }
}

