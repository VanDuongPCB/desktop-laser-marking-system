#include "HxLogWindow.h"
#include "ui_hxlogwindow.h"


HxLogWindow::HxLogWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::LogWindow )
{
    ui->setupUi( this );
    ui->toolBar->addWidget( new QLabel( tr( "Từ" ) ) );
    m_pDateFrom = new QDateEdit();
    m_pDateFrom->setCalendarPopup( true );
    m_pDateFrom->setDisplayFormat( "dd/MM/yyyy" );
    m_pDateFrom->setDate( QDate::currentDate() );
    ui->toolBar->addWidget( m_pDateFrom );
    ui->toolBar->addWidget( new QLabel( tr( "Đến" ) ) );
    m_pDateTo = new QDateEdit();
    m_pDateTo->setCalendarPopup( true );
    m_pDateTo->setDisplayFormat( "dd/MM/yyyy" );
    m_pDateTo->setDate( QDate::currentDate() );
    ui->toolBar->addWidget( m_pDateTo );

    ui->toolBar->addWidget( new QLabel( tr( "Số serial" ) ) );
    m_pSerial = new QLineEdit();
    m_pSerial->setSizePolicy( m_pDateTo->sizePolicy() );
    ui->toolBar->addWidget( m_pSerial );

    ui->toolBar->addActions( { ui->actionSearch, ui->actionExport } );

    QStringList columnNames = { "Thời gian","Serial","LOT","Model" };
    for ( int i = 1; i <= 10; i++ )
    {
        columnNames.push_back( tr( "Dữ liệu %1" ).arg( i ) );
    }

    ui->tbvLogs->setHeaders( columnNames );

    connect( ui->actionSearch, &QAction::triggered, this, &HxLogWindow::OnSearch );
    connect( ui->actionExport, &QAction::triggered, this, &HxLogWindow::OnExport );
}

HxLogWindow::~HxLogWindow()
{
    delete ui;
}

void HxLogWindow::OnSearch()
{
    QString serial = m_pSerial->text().trimmed().toUpper();
    if ( serial.isEmpty() )
        Logger()->Get( m_pDateFrom->date(), m_pDateTo->date(), m_logData );
    else
        Logger()->Get( serial, m_logData );


    ui->tbvLogs->setRowCount( m_logData.size() );
    int row = 0;
    for ( auto& log : m_logData )
    {
        ui->tbvLogs->setText( row, 0, log.Time );
        ui->tbvLogs->setText( row, 1, log.Serial );
        ui->tbvLogs->setText( row, 2, log.LOT );
        ui->tbvLogs->setText( row, 3, log.Model );
        for ( int i=0;i<log.items.size(); i++ )
        {
            ui->tbvLogs->setText( row, QString( "Dữ liệu %1" ).arg( i + 1 ), log.items[ i ] );
        }

        row++;
    }
}

void HxLogWindow::OnExport()
{
    Logger()->Export( m_logData, m_pDateFrom->date(), m_pDateTo->date() );
}