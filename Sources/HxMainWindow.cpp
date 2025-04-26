#include "HxMainWindow.h"
#include "ui_hxmainwindow.h"

#include "HxSettingWindow.h"
#include "HxMarkWindow.h"
#include "HxControlWindow.h"
#include "HxModelWindow.h"
#include "HxLOTWindow.h"
#include "HxDesignWindow.h"
#include "HxTransferWindow.h"
#include "HxLoginDialog.h"

#include "HxProtector.h"
#include "HxMessage.h"

#include "HxSystemError.h"



HxMainWindow::HxMainWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
    ui->setupUi( this );

    ui->pageSetting->layout()->addWidget( new HxSettingsWindow( this ) );
    ui->pagePass->layout()->addWidget( new HxTransferWindow( this ) );
    ui->pageMark->layout()->addWidget( new HxMarkWindow( this ) );
    ui->pageControl->layout()->addWidget( new HxControlWindow( this ) );
    ui->pageModel->layout()->addWidget( new HxModelWindow( this ) );
    ui->pageScheddule->layout()->addWidget( new HxLOTWindow( this ) );
    ui->pageDesign->layout()->addWidget( new HxDesignWindow( this ) );


    actions.push_back( ui->actionSettings );
    actions.push_back( ui->actionPass );
    actions.push_back( ui->actionMark );
    actions.push_back( ui->actionControl );
    actions.push_back( ui->actionLot );
    actions.push_back( ui->actionModel );
    actions.push_back( ui->actionDesign );
    actions.push_back( ui->actionIVProgram );

    pages.push_back( ui->pageSetting );
    pages.push_back( ui->pagePass );
    pages.push_back( ui->pageMark );
    pages.push_back( ui->pageControl );
    pages.push_back( ui->pageScheddule );
    pages.push_back( ui->pageModel );
    pages.push_back( ui->pageDesign );
    pages.push_back( ui->pageIV );

    for ( auto& action : actions )
    {
        connect( action, &QAction::toggled, this, &HxMainWindow::menuTabToggled );
    }
    ui->actionMark->setChecked( true );

    connect( HxSystemError::instance(), &HxSystemError::reported, this, &HxMainWindow::errorReported );
    connect( HxProtector::instance(), &HxProtector::loginChanged, this, &HxMainWindow::loginChanged );

    setWindowState( Qt::WindowMaximized );
}

HxMainWindow::~HxMainWindow()
{
    delete ui;
}

void HxMainWindow::setNavEnable( bool en )
{
    ui->toolBar->setEnabled( en );
}

void HxMainWindow::showEvent( QShowEvent* )
{

}

void HxMainWindow::closeEvent( QCloseEvent* )
{

}

void HxMainWindow::errorReported( HxException ex )
{
    HxMessage::error( ex.message, ex.where );
}

void HxMainWindow::loginChanged()
{
    updateUI();
}

void HxMainWindow::updateUI()
{
    HxUserProfile* user = HxProtector::instance()->currentUser();
    if ( user == nullptr )
    {
        ui->actionLogin->setText( "Đăng nhập" );
        ui->actionSettings->setEnabled( false );
        ui->actionControl->setEnabled( false );
        ui->actionLot->setEnabled( false );
        ui->actionModel->setEnabled( false );
        ui->actionDesign->setEnabled( false );
        ui->actionIVProgram->setEnabled( false );
    }
    else
    {
        ui->actionLogin->setText( "Đăng xuất" );
        bool isAdmin = user->isAdmin;
        bool isSuper = user->isSuper;
        bool isWorking = false;
        ui->actionSettings->setEnabled( isAdmin );
        ui->actionControl->setEnabled( isAdmin );
        ui->actionLot->setEnabled( isAdmin || isSuper );
        ui->actionModel->setEnabled( isAdmin );
        ui->actionDesign->setEnabled( isAdmin );
        ui->actionIVProgram->setEnabled( isAdmin );
    }
}

void HxMainWindow::menuTabToggled( bool )
{
    for ( int i = 0; i < actions.size(); i++ )
    {
        QAction* _sender = ( QAction* )sender();
        bool match = _sender == actions[ i ];
        disconnect( actions[ i ], &QAction::toggled, this, &HxMainWindow::menuTabToggled );
        if ( match )
        {
            ui->stackedWidget->setCurrentWidget( pages[ i ] );
            actions[ i ]->setChecked( true );
        }
        else
        {
            actions[ i ]->setChecked( false );
        }
        connect( actions[ i ], &QAction::toggled, this, &HxMainWindow::menuTabToggled );
    }
}


void HxMainWindow::on_actionLogin_triggered()
{
    if ( HxProtector::instance()->currentUser() != nullptr )
    {
        int res = HxMessage::warning( "Bạn có chắc chắn muốn đăng xuất không ?", "Khoan đã" );
        if ( res == QMessageBox::StandardButton::Yes )
        {
            HxProtector::instance()->logout();
            ui->actionMark->setChecked( true );
        }
    }
    else
    {
        HxLoginDialog dlg( this );
        if ( dlg.exec() )
        {

        }
    }
}

