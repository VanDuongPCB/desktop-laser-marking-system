#include "HxAddUserDialog.h"
#include "ui_hxadduserdialog.h"
#include "HxUserProfile.h"

HxAddUserDialog::HxAddUserDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::AddUserDialog )
{
    ui->setupUi( this );
}

HxAddUserDialog::~HxAddUserDialog()
{
    delete ui;
}

void HxAddUserDialog::on_btnAdd_clicked()
{
    QString user = ui->txtUser->text().trimmed();
    QString userLower = user.toLower();
    for ( auto& it : HxUserProfile::items )
    {
        if ( it->name.toLower() == userLower )
        {
            ui->lblError->setText( "Đã có user này !" );
            return;
        }
    }
    HxUserProfile* us = new HxUserProfile();
    us->name = user;

    HxUserProfile::items.emplace_back( us );
    this->close();
    this->setResult( 1 );
}


void HxAddUserDialog::on_txtUser_textChanged( const QString& arg1 )
{
    ui->btnAdd->setEnabled( false );
    QString user = arg1.trimmed().toLower();
    if ( user.length() < 1 )
    {

        return;
    }
    for ( auto& usr : HxUserProfile::items )
    {
        if ( usr->name == user )
        {
            return;
        }
    }
    ui->btnAdd->setEnabled( true );
}

