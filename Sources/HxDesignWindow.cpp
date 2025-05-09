#include "HxDesignWindow.h"
#include "ui_HxDesignWindow.h"

#include "QPushButton"

#include "HxModel.h"
#include "HxLOT.h"
#include "HxMessage.h"

HxDesignWindow::HxDesignWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::DesignWindow )
{
    ui->setupUi( this );

    ui->tbvDesign->setHeaders( { "C. trình","Rộng","Cao" } );
    ui->tbvDesign->setColumnWidth( 1, 60 );
    ui->tbvDesign->setColumnWidth( 2, 60 );
    ui->tbvDesign->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );

    ui->tbvBlocks->setHeaders( { "Block","Là mã ?","Độ dài","Nội dung" } );
    ui->tbvBlocks->setColumnWidth( 0, 80 );
    ui->tbvBlocks->setColumnWidth( 1, 80 );
    ui->tbvBlocks->setColumnWidth( 2, 80 );
    ui->tbvBlocks->setRowCount( m_maxBlockCount );

    ui->tbvModelParams->setHeaders( { "Thông số model" } );
    ui->tbvLotParams->setHeaders( { "Thông số lot" } );

    m_designs = DesignManager()->GetDesigns();

    ShowDesigns();
    ShowParams();
    ShowBlocks();

    connect( ui->actionSave, &QAction::triggered, this, &HxDesignWindow::OnSave );
    connect( ui->tbvDesign, &QTableView::pressed, this, &HxDesignWindow::OnSelected );
    connect( ui->tbvLotParams, &HxTableView::doubleClicked, this, &HxDesignWindow::OnInsertParam );
    connect( ui->tbvModelParams, &HxTableView::doubleClicked, this, &HxDesignWindow::OnInsertParam );

}

HxDesignWindow::~HxDesignWindow()
{
    delete ui;
}

void HxDesignWindow::showEvent( QShowEvent* )
{
    OnRefresh();
}

void HxDesignWindow::ShowDesigns()
{
    disconnect( ui->tbvDesign->dataTable(), &QStandardItemModel::itemChanged, this, &HxDesignWindow::OnSizeChanged );
    int rows = 2000;
    ui->tbvDesign->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        QString name = QString::number( row ).rightJustified( 4, '0' );
        ui->tbvDesign->setText( row, 0, name );
        auto flags = ui->tbvDesign->item( row, 0 )->flags();
        flags &= ~Qt::ItemIsEditable;
        ui->tbvDesign->item( row, 0 )->setFlags( flags );
        auto it = m_designs.find( name );
        if ( it != m_designs.end() )
        {
            ui->tbvDesign->setText( row, 1, QString::number( it->second->Width() ) );
            ui->tbvDesign->setText( row, 2, QString::number( it->second->Height() ) );
        }
        else
        {
            ui->tbvDesign->setText( row, 1, "" );
            ui->tbvDesign->setText( row, 2, "" );
        }
    }
    connect( ui->tbvDesign->dataTable(), &QStandardItemModel::itemChanged, this, &HxDesignWindow::OnSizeChanged );
}

void HxDesignWindow::ShowBlocks()
{
    auto& pDesign = m_pDesign;
    if ( !pDesign )
        pDesign = DesignManager()->Create();

    disconnect( ui->tbvBlocks->dataTable(), &QStandardItemModel::itemChanged, this, &HxDesignWindow::OnBlockChanged );
    for ( int row = 0; row < m_maxBlockCount; row++ )
    {
        HxBlock block = pDesign->Block( row );
        ui->tbvBlocks->setText( row, "Block", QString::number( row ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->item( row, "Block" )->setFlags( ui->tbvBlocks->item( row, 0 )->flags() & ~Qt::ItemIsEditable );

        ui->tbvBlocks->setText( row, "Là mã ?", "" );
        ui->tbvBlocks->item( row, "Là mã ?" )->setCheckable( true );
        ui->tbvBlocks->item( row, "Là mã ?" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
        ui->tbvBlocks->item( row, "Là mã ?" )->setFlags( ui->tbvBlocks->item( row, 1 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvBlocks->setText( row, "Độ dài", QString::number( block.textLen ) );
        ui->tbvBlocks->setText( row, "Nội dung", block.data );
    }
    connect( ui->tbvBlocks->dataTable(), &QStandardItemModel::itemChanged, this, &HxDesignWindow::OnBlockChanged );
}

void HxDesignWindow::ShowParams()
{
    // model
    QStringList modelParams = HxModel::paramNames();
    int rows = modelParams.size();
    ui->tbvModelParams->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvModelParams->setText( row, 0, modelParams[ row ] );
    }

    // lot
    QStringList lotParams = LOTManager()->Parameters();
    rows = lotParams.size();
    ui->tbvLotParams->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvLotParams->setText( row, 0, lotParams[ row ] );
    }
}

void HxDesignWindow::OnSelected( const QModelIndex& index )
{
    
    int row = index.row();
    if ( row < 0 )
        return;

    static int lastRow( -1 );
    if ( row != lastRow )
    {
        OnSave();
        lastRow = row;
    }

    QString designName = QString::number( row ).rightJustified( 4, '0' );
    m_pDesign = DesignManager()->GetDesign( designName );
    if ( !m_pDesign )
    {
        m_pDesign = DesignManager()->Create();
        m_pDesign->SetName( designName );
    }

    ShowBlocks();
}

void HxDesignWindow::OnSizeChanged( QStandardItem* item )
{
    if ( !m_pDesign )
        return;

    int col = item->column();
    bool bConverted;
    double value = item->text().toDouble( &bConverted );
    if ( bConverted )
    {
        if ( col == 1 )
            m_pDesign->SetWidth( value );
        else if ( col == 2 )
            m_pDesign->SetHeight( value );
    }

    if ( col == 1 )
        item->setText( QString::number( m_pDesign->Width() ) );
    else if ( col == 2 )
        item->setText( QString::number( m_pDesign->Height() ) );

}

void HxDesignWindow::OnBlockChanged( QStandardItem* item )
{
    if ( !m_pDesign )
        return;

    int row = item->row();
    HxBlock block;

    block.isCode = ui->tbvBlocks->item( row, "Là mã ?" )->checkState() == Qt::Checked;
    block.textLen = ui->tbvBlocks->item( row, "Độ dài" )->text().trimmed().toInt();
    block.data = ui->tbvBlocks->item( row, "Nội dung" )->text();
    m_pDesign->SetBlock( row + 1, block );
}

void HxDesignWindow::OnInsertParam( const QModelIndex& index )
{
    if ( index.row() < 0 )
        return;

    if ( ui->tbvBlocks->currentIndex().row() < 0 )
        return;

    HxTableView* pTableView = static_cast< HxTableView* >( sender() );
    QString paramName = pTableView->item( index.row(), 0 )->text();
    if ( pTableView == ui->tbvLotParams )
        paramName = "LOT." + paramName;
    else if ( pTableView == ui->tbvModelParams )
        paramName = "MODEL." + paramName;
    else 
        return;



    QString oldText = ui->tbvBlocks->item( ui->tbvBlocks->currentIndex().row(), "Nội dung" )->text();
    if ( oldText.length() > 0 ) 
        oldText += ",";
    oldText += paramName;
    ui->tbvBlocks->setText( ui->tbvBlocks->currentIndex().row(), "Nội dung", oldText );
}

void HxDesignWindow::OnRefresh()
{
    ShowBlocks();
}

void HxDesignWindow::OnSave()
{
    if ( !m_pDesign || !m_pDesign->IsMofified() )
        return;

    if ( HxMsgQuestion( tr( "Lưu dữ liệu mẫu tem: %1?" ).arg( m_pDesign->Name() ), tr( "Lưu dữ liệu" ) ) )
    {
        DesignManager()->Save( m_pDesign );
    }
}