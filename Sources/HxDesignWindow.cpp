#include "HxDesignWindow.h"
#include "ui_hxdesignwindow.h"
#include "HxModel.h"
#include "HxLOT.h"

HxDesignWindow::HxDesignWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::DesignWindow )
{
    ui->setupUi( this );

}

HxDesignWindow::~HxDesignWindow()
{
    delete ui;
}

void HxDesignWindow::showEvent( QShowEvent* )
{
    ShowDesigns( "" );
    ShowBlocks();
    ShowParams();
}

void HxDesignWindow::ShowDesigns( QString filter )
{
    designs.clear();
    filter = filter.trimmed().toUpper();
    if ( ui->tbvDesign->headers.empty() )
    {
        ui->tbvDesign->SetHeaders( { "Chương trình" } );
    }

    for ( auto& item : HxDesign::items )
    {
        if ( filter == "" || item->name.contains( filter ) )
        {
            designs.push_back( item );
        }
    }

    int rows = designs.size();
    ui->tbvDesign->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvDesign->SetText( row, 0, designs[ row ]->name );
    }
}

void HxDesignWindow::ShowSize()
{
    if ( design != nullptr )
    {
        ui->spxDesignWidth->setValue( design->width );
        ui->spxDesignHeight->setValue( design->height );
    }
    else
    {
        ui->spxDesignWidth->setValue( 2.5 );
        ui->spxDesignHeight->setValue( 2.5 );
    }
}

void HxDesignWindow::ShowBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->SetHeaders( { "Block","Là mã ?","Độ dài","Nội dung" } );
        ui->tbvBlocks->setColumnWidth( 0, 80 );
        ui->tbvBlocks->setColumnWidth( 1, 80 );
        ui->tbvBlocks->setColumnWidth( 2, 80 );
    }
    QStandardItemModel* model = ( QStandardItemModel* )ui->tbvBlocks->model();
    disconnect( model, &QStandardItemModel::itemChanged, this, &HxDesignWindow::BlockChanged );
    int rows = 32;
    ui->tbvBlocks->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvBlocks->SetText( row, "Block", QString::number( row ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->Item( row, "Block" )->setFlags( ui->tbvBlocks->Item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvBlocks->SetText( row, "Là mã ?", "" );
        ui->tbvBlocks->Item( row, "Là mã ?" )->setCheckable( true );
        ui->tbvBlocks->Item( row, "Là mã ?" )->setCheckState( Qt::Unchecked );
        ui->tbvBlocks->Item( row, "Là mã ?" )->setFlags( ui->tbvBlocks->Item( row, 1 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvBlocks->SetText( row, "Độ dài", "0" );
        ui->tbvBlocks->SetText( row, "Nội dung", "" );
    }
    if ( design != nullptr )
    {
        QList<int> nums = design->blocks.keys();
        for ( auto num : nums )
        {
            HxBlock block = design->blocks[ num ];
            ui->tbvBlocks->Item( num, "Là mã ?" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
            ui->tbvBlocks->SetText( num, "Độ dài", QString::number( block.textLen ) );
            ui->tbvBlocks->SetText( num, "Nội dung", block.data );
        }
    }
    connect( model, &QStandardItemModel::itemChanged, this, &HxDesignWindow::BlockChanged );
}

void HxDesignWindow::ShowParams()
{
    // model
    if ( ui->tbvModelParams->headers.empty() )
    {
        ui->tbvModelParams->SetHeaders( { "Thông số model" } );
    }
    QStringList modelParams = HxModel::ParamNames();
    int rows = modelParams.size();
    ui->tbvModelParams->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvModelParams->SetText( row, 0, modelParams[ row ] );
    }


    // lot
    if ( ui->tbvLotParams->headers.empty() )
    {
        ui->tbvLotParams->SetHeaders( { "Thông số lot" } );
    }
    QStringList lotParams = HxLOT::ParamNames();
    rows = lotParams.size();
    ui->tbvLotParams->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvLotParams->SetText( row, 0, lotParams[ row ] );
    }
}

void HxDesignWindow::BlockChanged( QStandardItem* item )
{
    if ( design == nullptr ) return;
    int row = item->row();
    bool code = ui->tbvBlocks->Item( row, "Là mã ?" )->checkState() == Qt::Checked;
    int len = ui->tbvBlocks->Item( row, "Độ dài" )->text().trimmed().toInt();
    QString data = ui->tbvBlocks->Item( row, "Nội dung" )->text();
    if ( !design->blocks.contains( row ) )
    {
        design->blocks.insert( row, HxBlock() );
    }
    design->blocks[ row ].isCode = code;
    design->blocks[ row ].textLen = len;
    design->blocks[ row ].data = data;
    HxDesign::Save( design );
}

void HxDesignWindow::on_actionSave_triggered()
{
    HxDesign::Save();
}

void HxDesignWindow::on_actionLoad_triggered()
{
    design = nullptr;
    HxDesign::Load();
    ShowDesigns( "" );
    ShowBlocks();
}

void HxDesignWindow::on_tbvDesign_pressed( const QModelIndex& index )
{
    int row = index.row();
    if ( row < 0 ) return;
    design = designs[ row ];
    ShowSize();
    ShowBlocks();
}

void HxDesignWindow::on_spxDesignWidth_valueChanged( double arg1 )
{
    if ( design == nullptr ) return;
    design->width = arg1;
    HxDesign::Save( design );
}

void HxDesignWindow::on_spxDesignHeight_valueChanged( double arg1 )
{
    if ( design == nullptr ) return;
    design->height = arg1;
    HxDesign::Save( design );
}

void HxDesignWindow::on_tbvModelParams_doubleClicked( const QModelIndex& index )
{
    int row = ui->tbvBlocks->currentIndex().row();
    if ( row < 0 ) return;
    QString param = "MODEL." + ui->tbvModelParams->Item( index.row(), 0 )->text();
    QString oldText = ui->tbvBlocks->Item( row, "Nội dung" )->text();
    if ( oldText.length() > 0 ) oldText += ",";
    oldText += param;
    ui->tbvBlocks->SetText( row, "Nội dung", oldText );
    HxDesign::Save( design );
}


void HxDesignWindow::on_tbvLotParams_doubleClicked( const QModelIndex& index )
{
    int row = ui->tbvBlocks->currentIndex().row();
    if ( row < 0 ) return;
    QString param = "LOT." + ui->tbvLotParams->Item( index.row(), 0 )->text();
    QString oldText = ui->tbvBlocks->Item( row, "Nội dung" )->text();
    if ( oldText.length() > 0 ) oldText += ",";
    oldText += param;
    ui->tbvBlocks->SetText( row, "Nội dung", oldText );
    HxDesign::Save( design );
}

