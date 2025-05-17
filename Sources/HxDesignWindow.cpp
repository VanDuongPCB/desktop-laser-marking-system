#include "HxDesignWindow.h"
#include "ui_HxDesignWindow.h"

#include "QPushButton"
#include "QLabel"

#include "HxModel.h"
#include "HxLOT.h"
#include "HxMessage.h"
#include "HxEvent.h"

namespace
{
    int s_maxBlockCount = 32;
}

HxDesignWindow::HxDesignWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::DesignWindow )
{
    ui->setupUi( this );
    m_pLblMessage = new QLabel();
    ui->toolBar->addWidget( m_pLblMessage );

    ui->tbvDesign->setHeaders( { "C. trình","Rộng","Cao" } );
    ui->tbvDesign->setColumnWidth( 1, 60 );
    ui->tbvDesign->setColumnWidth( 2, 60 );
    ui->tbvDesign->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );

    ui->tbvBlocks->setHeaders( { "Block","Là mã ?","Độ dài","Nội dung" } );
    ui->tbvBlocks->setColumnWidth( 0, 80 );
    ui->tbvBlocks->setColumnWidth( 1, 80 );
    ui->tbvBlocks->setColumnWidth( 2, 80 );
    ui->tbvBlocks->setRowCount( s_maxBlockCount );

    ui->tbvModelParams->setHeaders( { "Thông số model" } );
    ui->tbvLotParams->setHeaders( { "Thông số lot" } );

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
    m_designs = DesignManager()->GetDesigns();
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
    std::map<int, HxBlock> blocks;
    if ( m_pDesign )
        blocks = m_pDesign->Blocks();

    disconnect( ui->tbvBlocks->dataTable(), &QStandardItemModel::itemChanged, this, &HxDesignWindow::OnBlockChanged );
    for ( int row = 0; row < s_maxBlockCount; row++ )
    {
        HxBlock block = blocks[ row ];
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
    QString designName = ui->tbvDesign->item( row, 0 )->text().replace( "*", "" );
    auto it = m_designChanges.find( designName );
    if ( it != m_designChanges.end() )
    {
        m_pDesign = m_designChanges[ designName ];
    }
    else
    {
        m_pDesign = DesignManager()->GetDesign( designName );
    }
    ShowBlocks();
}

void HxDesignWindow::OnSizeChanged( QStandardItem* item )
{
    int row = item->row();
    QString designName = ui->tbvDesign->item( row, 0 )->text().replace( "*", "" );
    double wvalue = ui->tbvDesign->item( row, 1 )->text().toDouble();
    double hvalue = ui->tbvDesign->item( row, 2 )->text().toDouble();

    if ( !m_pDesign )
    {
        m_pDesign = DesignManager()->GetDesign( designName );
        if ( !m_pDesign )
        {
            m_pDesign = DesignManager()->Create();
            m_pDesign->SetName( designName );
            qDebug() << m_pDesign->ModifyFlags();
        }
    }
    m_pDesign->SetWidth( wvalue );
    m_pDesign->SetHeight( hvalue );

    QString designNameChanged = designName + ( m_pDesign->IsMofified() ? "*" : "" );
    QSignalBlocker blocker( ui->tbvDesign->dataTable() );
    ui->tbvDesign->setText( row, 0, designNameChanged );
    ui->tbvDesign->setText( row, 1, QString::number( wvalue ) );
    ui->tbvDesign->setText( row, 2, QString::number( hvalue ) );

    if ( m_pDesign->IsMofified() && m_designChanges.find( designName ) == m_designChanges.end() )
    {
        m_designChanges[ designName ] = m_pDesign;
    }
}

void HxDesignWindow::OnBlockChanged( QStandardItem* item )
{
    int row = item->row();
    if ( row < 1 )
    {
        item->setText( "" );
        return;
    }
    if ( ui->tbvDesign->currentIndex().row() < 0 )
    {
        HxMsgWarning( tr( "Hãy chọn một chương trình trước." ) );
        QSignalBlocker blocker( ui->tbvBlocks->dataTable() );
        item->setText( "" );
        return;
    }

    QString designName = ui->tbvDesign->item( ui->tbvDesign->currentIndex().row(), 0 )->text().replace( "*", "" );

    if ( !m_pDesign )
    {
        m_pDesign = DesignManager()->Create();
        m_pDesign->SetName( designName );
        m_designChanges[ designName ] = m_pDesign;
    }

    
    HxBlock block;

    block.isCode = ui->tbvBlocks->item( row, "Là mã ?" )->checkState() == Qt::Checked;
    block.textLen = ui->tbvBlocks->item( row, "Độ dài" )->text().trimmed().toInt();
    block.data = ui->tbvBlocks->item( row, "Nội dung" )->text();
    m_pDesign->SetBlock( row, block );

    QString designNameChanged = designName + ( m_pDesign->IsMofified() ? "*" : "" );
    int designRow = ui->tbvDesign->currentIndex().row();
    ui->tbvDesign->setText( designRow, 0, designNameChanged );
    ui->tbvDesign->setText( designRow, 1, QString::number( m_pDesign->Width() ) );
    ui->tbvDesign->setText( designRow, 2, QString::number( m_pDesign->Height() ) );
    ui->tbvDesign->update( ui->tbvDesign->dataTable()->index( designRow, 0 ) );
    ui->tbvDesign->update( ui->tbvDesign->dataTable()->index( designRow, 1 ) );
    ui->tbvDesign->update( ui->tbvDesign->dataTable()->index( designRow, 2 ) );
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
    ShowDesigns();
    ShowParams();
    ShowBlocks();
}

void HxDesignWindow::OnSave()
{
    if ( m_designChanges.size() < 1 )
        return;

    if ( HxMsgQuestion( tr( "Lưu dữ liệu mẫu tem bị thay đổi?" ), tr( "Lưu dữ liệu" ) ) )
    {
        int saveCnt = 0;
        for ( auto &[name, design] : m_designChanges )
        {
            if ( DesignManager()->Save( m_pDesign ) == RtNormal )
                saveCnt++;
        }
        if(saveCnt>0 )
            qApp->postEvent( qApp, new HxEvent( HxEvent::eDesignChanged ) );
        m_designChanges.clear();
        ShowDesigns();
    }
}