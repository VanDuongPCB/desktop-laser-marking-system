#include "HxIVProgramWindow.h"
#include "ui_hxivprogramwindow.h"

#include "QDir"

HxIVProgramWindow::HxIVProgramWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::IVWindow )
{
    ui->setupUi( this );

    ui->tableView->setHeaders( { "Chương trình","Dung lượng", "Sửa đổi", "Mô tả" } );
    ui->tableView->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );

    connect( ui->actionLoad, &QAction::triggered, this, &HxIVProgramWindow::OnLoad );
    OnLoad();
}

HxIVProgramWindow::~HxIVProgramWindow()
{
    delete ui;
}


void HxIVProgramWindow::OnLoad()
{
    HxRegistrySetting setting;
    QString ivDir = setting.String( IVProgramDir );

    QStringList files = QDir( ivDir ).entryList( { "*.iva" }, QDir::Files | QDir::NoSymLinks );
    ui->tableView->setRowCount( files.size() );
    for ( int i = 0; i < files.size(); i++ )
    {
        QFileInfo fi( ivDir + "/" + files[i]);
        ui->tableView->setText( i, 0, fi.absoluteFilePath() );
        ui->tableView->setText( i, 1, QString( "%1 MB" ).arg( fi.size() / 1024.0 / 1024.0 ) );
        ui->tableView->setText( i, 2, fi.lastModified().toString( "yyyy-MM-dd" ) );
    }

}