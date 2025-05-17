#include "HxMigrationDialog.h"
#include "ui_hxmigrationdialog.h"

#include "QFileDialog"

#include "HxMigration.h"


HxMigrationDialog::HxMigrationDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::MigrationDialog )
{
    ui->setupUi( this );

    ui->txtDBFile->setText( m_settings.String( DatabaseFilePath ) );

    connect( ui->btnBrowseDataDir, &QPushButton::clicked, this, &HxMigrationDialog::OnBrowseOldDataDir );
    connect( ui->btnMigrateLOTs, &QPushButton::clicked, this, &HxMigrationDialog::OnMigrationLOTs );
    connect( ui->btnMigrateModels, &QPushButton::clicked, this, &HxMigrationDialog::OnMigrationModels );
    connect( ui->btnMigrateDesigns, &QPushButton::clicked, this, &HxMigrationDialog::OnMigrationDesigns );
    connect( ui->btnMigrateStoppers, &QPushButton::clicked, this, &HxMigrationDialog::OnMigrationStoppers );
}
HxMigrationDialog::~HxMigrationDialog()
{
    delete ui;
}

void HxMigrationDialog::OnBrowseOldDataDir()
{
    QString dir = QFileDialog::getExistingDirectory( this, tr("Chọn thư mục chương trình IV"), ui->txtRootDir->text() );
    if ( !dir.isEmpty() )
    {
        ui->txtRootDir->setText( dir );
        ui->txtLOTDir->setText( dir + "/data/LOTS" );
        ui->txtModelDir->setText( dir + "/data/MODELS" );
        ui->txtDesinDir->setText( dir + "/data/DESIGNS" );
        ui->txtPrintLogDir->setText( dir + "/data/PRINT-LOGS" );
        ui->txtRePrintLogDir->setText( dir + "/data/REPRINT-LOGS" );
        ui->txtBarcodeDir->setText( dir + "/data/BARCODE-LOGS" );
        ui->txtStopperFile->setText( dir + "/settings/stoppers.json" );
    }
}

void HxMigrationDialog::OnMigrationLOTs()
{
    MigrateLOTs( ui->txtLOTDir->text() );
}

void HxMigrationDialog::OnMigrationModels()
{
    MigrateModels( ui->txtModelDir->text() );
}

void HxMigrationDialog::OnMigrationDesigns()
{
    MigrateDesigns( ui->txtDesinDir->text() );
}

void HxMigrationDialog::OnMigrationPrintLogs()
{
    MigratePrintLogs( ui->txtPrintLogDir->text() );
}

void HxMigrationDialog::OnMigrationRePrintLogs()
{
    MigrateRePrintLogs( ui->txtRePrintLogDir->text() );
}

void HxMigrationDialog::OnMigrationBarcodes()
{
    MigrateBarcodes( ui->txtBarcodeDir->text() );
}

void HxMigrationDialog::OnMigrationStoppers()
{
    MigrateStoppers( ui->txtStopperFile->text() );
}