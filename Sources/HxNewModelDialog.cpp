#include "HxNewModelDialog.h"
#include "ui_hxnewmodeldialog.h"
#include "HxModel.h"


HxNewModelDialog::HxNewModelDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::NewModelDialog )
{
    ui->setupUi( this );
}

HxNewModelDialog::~HxNewModelDialog()
{
    delete ui;
}

bool HxNewModelDialog::checkInputs()
{
    QString name = ui->txtModelName->text().trimmed().toUpper();
    QString code = ui->txtModelCode->text().trimmed().toUpper();

    if ( name.length() < 1 || code.length() < 1 ) return false;
    for ( auto& md : HxModel::items )
    {
        if ( md->name == name || md->code == code )
        {
            return false;
        }
    }
    return true;
}

void HxNewModelDialog::on_btnCreate_clicked()
{
    QString name = ui->txtModelName->text().trimmed().toUpper();
    QString code = ui->txtModelCode->text().trimmed().toUpper();
    QString no = ui->txtModelNo->text().trimmed().toUpper();

    data = HxModel::create( name, code );
    data->kNo = no;
    data->cvWidth = ui->spxCvWidth->value();
    HxModel::save( data );

    this->close();
    this->setResult( 1 );
}

void HxNewModelDialog::on_txtModelCode_textChanged( const QString& arg1 )
{
    ui->btnCreate->setEnabled( checkInputs() );
}


void HxNewModelDialog::on_txtModelName_textChanged( const QString& arg1 )
{
    ui->btnCreate->setEnabled( checkInputs() );
}

