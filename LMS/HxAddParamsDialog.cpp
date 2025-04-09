#include "HxAddParamsDialog.h"
#include "ui_HxAddParamsDialog.h"

HxAddParamsDialog::HxAddParamsDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::AddParamsDialog )
{
    ui->setupUi( this );
}

HxAddParamsDialog::~HxAddParamsDialog()
{
    delete ui;
}

void HxAddParamsDialog::on_btnAdd_clicked()
{
    QString text = ui->txtParamNames->toPlainText().trimmed().toUpper();
    QStringList items = text.split( ',' );
    m_names.clear();
    for ( auto& it : items )
    {
        QString item = it.trimmed();
        if ( item.length() <= 0 ) continue;
        if ( m_names.contains( item ) ) continue;
        m_names.push_back( item );
    }

    if ( m_names.size() > 0 )
    {
        m_bIsApplyAll = ui->chxApplyAll->isChecked();
        m_bIsDefault = ui->chxDefault->isChecked();
        this->close();
        this->setResult( 1 );
    }
}

