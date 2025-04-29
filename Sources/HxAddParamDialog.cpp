#include "HxAddParamDialog.h"
#include "ui_hxaddparamdialog.h"

HxAddParamDialog::HxAddParamDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::AddParamDialog )
{
    ui->setupUi( this );
}

HxAddParamDialog::~HxAddParamDialog()
{
    delete ui;
}

void HxAddParamDialog::on_btnAdd_clicked()
{
    QString text = ui->txtParamNames->toPlainText().trimmed().toUpper();
    QStringList items = text.split( ',' );
    names.clear();
    for ( auto& it : items )
    {
        QString item = it.trimmed();
        if ( item.length() <= 0 ) continue;
        if ( names.contains( item ) ) continue;
        names.push_back( item );
    }

    if ( names.size() > 0 )
    {
        isApplyAll = ui->chxApplyAll->isChecked();
        isDefault = ui->chxDefault->isChecked();
        this->close();
        this->setResult( 1 );
    }
}

