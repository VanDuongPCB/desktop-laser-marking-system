#include "HxAddProfileDialog.h"
#include "ui_HxAddProfileDialog.h"
#include "HxProfile.h"

HxAddProfileDialog::HxAddProfileDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::AddUserDialog )
{
    ui->setupUi( this );
}

HxAddProfileDialog::~HxAddProfileDialog()
{
    delete ui;
}

void HxAddProfileDialog::on_btnAdd_clicked()
{
    HxProfilePtrArray items = GetProfileManager()->GetProfiles();
    QString user = ui->txtUser->text().trimmed();
    QString userLower = user.toUpper();
    for ( auto& it : items )
    {
        if ( it->name.toUpper() == userLower )
        {
            ui->lblError->setText( "Đã có user này !" );
            return;
        }
    }
    HxProfilePtr us = std::make_shared<HxProfile>();
    us->name = user;
    us->pass = user;
    GetProfileManager()->Save(us);
    this->close();
    this->setResult( 1 );
}


void HxAddProfileDialog::on_txtUser_textChanged( const QString& arg1 )
{
    ui->btnAdd->setEnabled( false );
    // QString user = arg1.trimmed().toLower();
    // if ( user.length() < 1 )
    // {

    //     return;
    // }
    // for ( auto& usr : HxProfile::items )
    // {
    //     if ( usr->name == user )
    //     {
    //         return;
    //     }
    // }
    ui->btnAdd->setEnabled( true );
}

