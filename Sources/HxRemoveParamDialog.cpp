#include "HxRemoveParamDialog.h"
#include "ui_hxremoveparamdialog.h"

HxRemoveParamDialog::HxRemoveParamDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::RemoveParamDialog )
{
    ui->setupUi( this );
}

HxRemoveParamDialog::~HxRemoveParamDialog()
{
    delete ui;
}

void HxRemoveParamDialog::SetParams( QStringList names )
{
    ui->txtParamNames->setPlainText( names.join( ',' ) );
}

void HxRemoveParamDialog::on_btnRemove_clicked()
{
    isApplyAll = ui->chxApplyAll->isChecked();
    this->close();
    this->setResult( 1 );
}

