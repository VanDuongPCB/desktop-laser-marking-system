#include "HxMainWindow.h"
#include "ui_hxmainwindow.h"

#include "QSignalBlocker"
#include "QCloseEvent"

#include "HxSettingsWindow.h"
#include "HxMarkWindow.h"
#include "HxControlWindow.h"
#include "HxModelWindow.h"
#include "HxLOTWindow.h"
#include "HxDesignWindow.h"
#include "HxTransferWindow.h"
#include "HxLoginDialog.h"

#include "HxProtector.h"
#include "HxMessage.h"
#include "HxEvent.h"
#include "HxSystemError.h"
#include "HxLicense.h"
#include "HxMarker.h"


HxMainWindow::HxMainWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
    ui->setupUi( this );

    ui->tabMark->layout()->addWidget( new HxMarkWindow() );
    ui->tabTransfer->layout()->addWidget( new HxTransferWindow() );
    ui->tabPlans->layout()->addWidget( new HxLOTWindow() );
    ui->tabModel->layout()->addWidget( new HxModelWindow() );
    ui->tabDesign->layout()->addWidget( new HxDesignWindow() );
    //ui->tabIV
    ui->tabSetting->layout()->addWidget( new HxSettingsWindow() );
    ui->tabControl->layout()->addWidget( new HxControlWindow() );

    qApp->installEventFilter( this );

    m_currentTabIndex = ui->tabWidget->currentIndex();

    connect( HxSystemError::Instance(), &HxSystemError::Reported, this, &HxMainWindow::ErrorReported );
    connect( ui->tabWidget, &QTabWidget::currentChanged, this, &HxMainWindow::OnTabChanged );

    OnLockUI();
    setWindowState( Qt::WindowMaximized );

    Marker()->Init();
}

HxMainWindow::~HxMainWindow()
{
    Marker()->DeInit();
    delete ui;
}

void HxMainWindow::resizeEvent( QResizeEvent* event )
{
    if ( !m_pLblVersion )
    {
        m_pLblVersion = new QLabel( this );
        m_pLblVersion->setGeometry( 0, 0, 100, 35 );
        m_pLblVersion->setText( tr("Phiên bản\n") + Licensing()->GetVersion());
        m_pLblVersion->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
        m_pLblVersion->setStyleSheet( "color:#808; font-weight:bold;" );
    }
    QRect mainGeo = geometry();
    m_pLblVersion->setGeometry( mainGeo.width() - m_pLblVersion->width()-5, 5, 100, 35 );
}

void HxMainWindow::closeEvent( QCloseEvent* event)
{
    if ( HxMsgQuestion( tr( "Xác nhận thoát chương trình" ), tr( "Thoát chương trình?" ) ) != HxMsgButton::Yes )
    {
        event->ignore();
        return;
    }
}

bool HxMainWindow::eventFilter( QObject* watched, QEvent* event )
{
    HxEvent* hxEvent( nullptr );
    HxEvent::Type type;
    if ( !HxEvent::IsCustomEvent( event, hxEvent, type ) )
        return QMainWindow::eventFilter( watched, event );

    switch ( type )
    {
    case HxEvent::eLoginEvent:
    case HxEvent::eLogoutEvent:
    {
        HxProfilePtr pProfile = Protector()->Profile();
        if ( !pProfile )
            OnLockUI();
        else if ( pProfile->Permission( "ADMIN" ) )
            OnUnLockUIForAdmin();
        else if ( pProfile->Permission( "SUPER" ) )
            OnUnLockUIForSuper();
        else if ( pProfile->Permission( "LEADER" ) )
            OnUnLockUIForLeader();
    }
        break;
    default:
        break;
    }

    return QMainWindow::eventFilter( watched, event );
}

void HxMainWindow::OnLoginOrLogout()
{
    if ( Protector()->Profile() != nullptr )
    {
        int res = HxMsgQuestion( "Bạn có chắc chắn muốn đăng xuất không ?", "Khoan đã" );
        if ( res == QMessageBox::StandardButton::Yes )
            Protector()->Logout();
    }
    else
    {
        HxLoginDialog( this ).exec();
    }
}

void HxMainWindow::OnLockUI()
{
    OnUnLockUIForAdmin();
    ui->tabWidget->setTabText( 0, tr( "Đăng nhập" ) );
    ui->tabWidget->setTabVisible( 3, false );
    ui->tabWidget->setTabVisible( 4, false );
    ui->tabWidget->setTabVisible( 5, false );
    ui->tabWidget->setTabVisible( 6, false );
    ui->tabWidget->setTabVisible( 7, false );
    ui->tabWidget->setTabVisible( 8, false );
    ui->tabWidget->setTabVisible( 9, false );
}

void HxMainWindow::OnUnLockUIForLeader()
{
    OnUnLockUIForAdmin();
    ui->tabWidget->setTabVisible( 7, false );
    ui->tabWidget->setTabVisible( 8, false );
}

void HxMainWindow::OnUnLockUIForSuper()
{
    OnUnLockUIForAdmin();
    ui->tabWidget->setTabVisible( 7, false );
    ui->tabWidget->setTabVisible( 8, false );
}

void HxMainWindow::OnUnLockUIForAdmin()
{
    ui->tabWidget->setTabText( 0, tr( "Đăng xuất" ) );
    for ( int i = 0; i < ui->tabWidget->count(); i++ )
        ui->tabWidget->setTabVisible( i, true );
}

void HxMainWindow::OnTabChanged( int index )
{
    if ( !ui->tabWidget->currentWidget()->isEnabled() || index == 0 )
    {
        QSignalBlocker blocker( ui->tabWidget );
        ui->tabWidget->setCurrentIndex( m_currentTabIndex );
        if ( index == 0 )
            OnLoginOrLogout();
        return;
    }

    m_currentTabIndex = index;
}

void HxMainWindow::ErrorReported( HxException ex )
{
    HxMsgError( ex.Message(), ex.Where());
}
