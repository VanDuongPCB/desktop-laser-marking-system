#include "HxRemoveParamDialog.h"
#include "ui_HxRemoveParamDialog.h"

HxRemoveParamDialog::HxRemoveParamDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::RemoveParamDialog )
{
    ui->setupUi( this );
}

HxRemoveParamDialog::~HxRemoveParamDialog()
{
    delete ui;
}

void HxRemoveParamDialog::setParams( QStringList names )
{
    ui->txtParamNames->setPlainText( names.join( ',' ) );
}

void HxRemoveParamDialog::on_btnRemove_clicked()
{
    isApplyAll = ui->chxApplyAll->isChecked();
    this->close();
    this->setResult( 1 );
}

