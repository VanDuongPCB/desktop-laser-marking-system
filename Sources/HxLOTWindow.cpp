#include "HxLOTWindow.h"
#include "ui_hxlotwindow.h"
#include "HxLOTPropertyDialog.h"

#include "QLabel"

#include "HxMessage.h"
#include "HxMarker.h"
#include "HxConvert.h"

HxLOTWindow::HxLOTWindow( QWidget* parent ) :QMainWindow( parent ), ui( new Ui::LotWindow )
{
    ui->setupUi( this );

    ui->tbvLots->setHeaders( { "Tên","Kiểu in","Model","Sản lượng","Seri đầu","Seri cuối","Tiến độ","MAC đầu","MAC cuối","Trạng thái" } );
    ui->tbvLots->setColumnWidth( 0, 200 );
    ui->tbvLots->setColumnWidth( 2, 250 );
    ui->tbvLots->setColumnWidth( 7, 120 );
    ui->tbvLots->setColumnWidth( 8, 120 );
    ui->tbvLots->setColumnWidth( 9, 120 );

    QLabel* pLabel = new QLabel( "Tìm kiếm nhanh" );
    ui->toolBar->addWidget( pLabel );
    m_pSearchTextBox = new QLineEdit();
    m_pSearchTextBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    m_pSearchTextBox->setMinimumWidth( 300 );
    m_pSearchTextBox->setClearButtonEnabled( true );
    ui->toolBar->addWidget( m_pSearchTextBox );

    connect( ui->actionLoad, &QAction::triggered, this, &HxLOTWindow::OnRefresh );
    connect( m_pSearchTextBox, &QLineEdit::textChanged, this, &HxLOTWindow::OnFilter );
    connect( ui->actionNew, &QAction::triggered, this, &HxLOTWindow::OnNew );
    connect( ui->actionRemove, &QAction::triggered, this, &HxLOTWindow::OnDelete );
    connect( ui->actionSave, &QAction::triggered, this, &HxLOTWindow::OnSave );
    connect( ui->tbvLots, &HxTableView::doubleClicked, this, &HxLOTWindow::OnEdit );
}

HxLOTWindow::~HxLOTWindow()
{
    delete m_pSearchTextBox;
    delete ui;
}

void HxLOTWindow::showEvent( QShowEvent* )
{
    OnRefresh();
}

void HxLOTWindow::OnRefresh()
{
    OnSave();
    m_lotToSaves.clear();
    m_LOTs = LOTManager()->GetLOTs();
    OnFilter( m_pSearchTextBox->text() );
}

void HxLOTWindow::OnFilter( const QString& filter )
{
    QString input = filter.trimmed().toUpper();
    HxLOTPtrMap items;
    for ( auto& [name, pLOT] : m_LOTs )
    {
        bool bIsMatch = pLOT->Name().contains( input );

        if ( bIsMatch )
            items[ name ] = pLOT;
    }
    ui->tbvLots->setRowCount( items.size() );
    int row = 0;
    for ( auto& [name, pLOT] : items )
    {
        auto status = pLOT->Status();
        QString statusName = ProductStatusToString( status );
        QColor statusColor = ProductStatusToColor( status );

        ui->tbvLots->setText( row, "Tên", pLOT->Name() + ( pLOT->IsMofified() ? "*" : "" ) );
        ui->tbvLots->item( row, 0 )->setAccessibleText( name );
        ui->tbvLots->setText( row, "Kiểu in", pLOT->IsRePrint() ? "In lại" : "" );
        ui->tbvLots->setText( row, "Model", pLOT->Model() );
        ui->tbvLots->setText( row, "Sản lượng", QString::number( pLOT->Quantity() ) );
        ui->tbvLots->setText( row, "Seri đầu", pLOT->CounterStart() );
        ui->tbvLots->setText( row, "Seri cuối", pLOT->CounterEnd() );
        ui->tbvLots->setText( row, "Tiến độ", QString::number( pLOT->Progress() ) + "/" + QString::number( pLOT->Quantity() ) );
        ui->tbvLots->setText( row, "MAC đầu", pLOT->MACStart() );
        ui->tbvLots->setText( row, "MAC cuối", pLOT->MACEnd() );
        ui->tbvLots->setText( row, "Trạng thái", statusName );

        for ( int col = 0; col < ui->tbvLots->dataTable()->columnCount(); col++ )
        {
            ui->tbvLots->item( row, col )->setBackground( statusColor );
        }

        row++;
    }
}

void HxLOTWindow::OnNew()
{
    HxLOTPropertyDialog dialog( nullptr, m_LOTs, this );
    if ( dialog.exec() )
    {
        HxLOTPtr pLOT = dialog.GetLOT();
        m_lotToSaves[ "1_" + pLOT->Name()] = pLOT;
        m_LOTs[ "1_" + pLOT->Name() ] = pLOT;
        m_pSearchTextBox->setText( pLOT->Name() );
        OnFilter( pLOT->Name() );
    }
}

void HxLOTWindow::OnDelete()
{
    std::map<QString, QString> lotToDeletes;

    for ( auto& it : ui->tbvLots->selectionModel()->selectedRows() )
    {
        QStandardItem* item = ui->tbvLots->item( it.row(), 0 );
        lotToDeletes[ item->accessibleText() ] = item->text().replace( "*", "" );
    }

    if ( lotToDeletes.empty() )
        return;

    if ( HxMsgQuestion( tr( "Xóa các lot đang chọn?" ), tr( "Khoan đã" ) ) == QMessageBox::Yes )
    {
        for ( auto& [mask, name] : lotToDeletes )
        {
            LOTManager()->Remove( name );
            m_LOTs.erase( mask );
            m_lotToSaves.erase( mask );
        }
        OnFilter( m_pSearchTextBox->text() );
        // thêm event để cho các module khác kiểm tra
    }
}

void HxLOTWindow::OnEdit( const QModelIndex& index )
{
    int row = index.row();
    if ( row < 0 )
        return;


    QString lotNameMask = ui->tbvLots->item( row, 0 )->accessibleText().replace( "*", "" );
    auto it = m_LOTs.find( lotNameMask );
    if ( it == m_LOTs.end() )
        return;

    // chỗ này cần kiểm tra nếu nó được chỉnh sửa rồi thì không load lại nữa
    HxLOTPtr pLOT = it->second;
    if ( !pLOT->IsMofified() )
    {
        pLOT = LOTManager()->GetLOT( pLOT->Name() );
        m_LOTs[ lotNameMask ] = pLOT;
    }

    HxLOTPropertyDialog dialog( pLOT, m_LOTs, this );
    if ( dialog.exec() )
    {
        HxLOTPtr pLOT = dialog.GetLOT();
        m_LOTs[ lotNameMask ] = pLOT;
        m_lotToSaves[ lotNameMask ] = pLOT;
        OnFilter( m_pSearchTextBox->text() );
    }
}

void HxLOTWindow::OnSave()
{
    if ( m_lotToSaves.empty() )
        return;

    if ( HxMsgQuestion( tr( "Lưu %1 LOT đang thay đổi?" ).arg( m_lotToSaves.size() ), tr( "Khoan đã" ) ) == QMessageBox::Yes )

        for ( auto& [mask, pLOT] : m_lotToSaves )
        {
            LOTManager()->Save( pLOT );
        }
    m_lotToSaves.clear();
    OnFilter( m_pSearchTextBox->text() );
}