#include "HxModelWindow.h"
#include "ui_hxmodelwindow.h"
#include "QSpacerItem"
#include "QFileDialog"

#include "QSignalBlocker"

#include "HxAddParamsDialog.h"
#include "HxNewModelDialog.h"

#include "HxIVProgram.h"
#include "HxDesign.h"
#include "HxRemoveParamDialog.h"

#include "HxMessage.h"

namespace
{
    int s_maxPointSupported = 60;
}

HxModelWindow::HxModelWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::ModelWindow )
{
    ui->setupUi( this );
    // search
    QLabel* pLabel = new QLabel( "Tìm kiếm nhanh" );
    ui->toolBar->addWidget( pLabel );
    m_pSearchTextBox = new QLineEdit();
    m_pSearchTextBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    m_pSearchTextBox->setMinimumWidth( 300 );
    m_pSearchTextBox->setClearButtonEnabled( true );
    ui->toolBar->addWidget( m_pSearchTextBox );

    connect( m_pSearchTextBox, &QLineEdit::textChanged, this, &HxModelWindow::OnFilter );

    ui->cbxIVPrograms->addItem( "" );
    ui->cbxIVPrograms->addItems( HxIVProgram::names() );

    //connect( ui->actionNew, &QAction::triggered, this, &HxModelWindow::OnNew );
    //connect( ui->actionRemove, &QAction::triggered, this, &HxModelWindow::OnRemove );
    //connect( ui->actionSave, &QAction::triggered, this, &HxModelWindow::OnSave );
    //connect( ui->actionLoad, &QAction::triggered, this, &HxModelWindow::OnRefresh );
    //connect( txtSearch, &QLineEdit::textChanged, this, &HxModelWindow::OnFilter );
    //connect( txtSearch, &QLineEdit::editingFinished, this, &HxModelWindow::OnFilter );

    ui->tbvModels->setHeaders( { "Mã","Tên",".No","Băng chuyền","Chương trình IV" } );
    ui->tbvModels->setColumnWidth( 1, 350 );
    ui->tbvModels->setColumnWidth( 4, 350 );

    ui->tbvMarkPos->setHeaders( { "X","Y","Góc" } );
    ui->tbvComments->setHeaders( { "Thông tin","Giá trị" } );
    ui->tbvBlocks->setHeaders( { "Block","Mã","Độ dài","Giá trị" } );
    ui->tbvBlocks->setColumnWidth( 0, 60 );
    ui->tbvBlocks->setColumnWidth( 1, 60 );
    ui->tbvBlocks->setColumnWidth( 2, 60 );

    connect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::OnSelect );
    
    connect( ui->txtModelCode, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged );
    connect( ui->txtModelName, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged );
    connect( ui->txtModelNo, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged );
    connect( ui->cbxStopper, &QComboBox::currentIndexChanged, this, &HxModelWindow::OnInfoChanged );
    connect( ui->spxProgram, &QSpinBox::editingFinished, this, &HxModelWindow::OnInfoChanged );
    connect( ui->cbxIVPrograms, &QComboBox::currentIndexChanged, this, &HxModelWindow::OnInfoChanged );
    connect( ui->spxCvWidth, &QDoubleSpinBox::editingFinished, this, &HxModelWindow::OnInfoChanged );
    connect( ui->chxPrintLo, &QCheckBox::toggled, this, &HxModelWindow::OnInfoChanged );

    
    connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
    connect( ui->tbvMarkPos->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnPositionChanged );

    connect( ui->actionNew, &QAction::triggered, this, &HxModelWindow::OnNew );
    connect( ui->actionRemove, &QAction::triggered, this, &HxModelWindow::OnRemove );
    connect( ui->actionSave, &QAction::triggered, this, &HxModelWindow::OnSave );
    connect( ui->btnAddParam, &QPushButton::clicked, this, &HxModelWindow::OnAddParam );
    connect( ui->btnRemoveParam, &QPushButton::clicked, this, &HxModelWindow::OnRemoveParam );
}

HxModelWindow::~HxModelWindow()
{
    delete m_pSearchTextBox;
    delete ui;
}

void HxModelWindow::showEvent( QShowEvent* )
{
    OnRefresh();
}

void HxModelWindow::ShowModels()
{
    QSignalBlocker blocker( this );
    int rows = m_models.size();
    ui->tbvModels->setRowCount( rows );
    int row = 0;
    for ( auto& [name, pModel] : m_models )
    {
        ui->tbvModels->setText( row, "Mã", pModel->Code() );
        ui->tbvModels->setText( row, "Tên", pModel->Name() );
        ui->tbvModels->setText( row, ".No", pModel->kNo() );
        ui->tbvModels->setText( row, "Băng chuyền", QString::number( pModel->CvWidth() ) );
        ui->tbvModels->setText( row, "Chương trình IV", pModel->IVProgram() );
        row++;
    }
}

void HxModelWindow::ShowModelInfo()
{
    QSignalBlocker bocker( ui->widInfo );
    auto& model = m_pModel;
    if ( !model )
        model = ModelManager()->Create();
    ui->txtModelName->setText( model->Name() );
    ui->txtModelCode->setText( model->Code() );
    ui->txtModelNo->setText( model->kNo() );

    // iv
    ui->cbxIVPrograms->blockSignals( true );
    ui->cbxIVPrograms->setCurrentText( model->IVProgram() );
    ui->cbxIVPrograms->setEditText( model->IVProgram() );
    ui->cbxIVPrograms->blockSignals( false );
    qDebug() << model->IVProgram();

    // design
    ui->spxProgram->setEnabled( false );
    ui->spxProgram->setValue( model->Design().toInt( 0 ) );
    ui->spxProgram->setEnabled( true );

    ui->spxCvWidth->setValue( model->CvWidth() );

    ui->cbxStopper->setEnabled( false );
    ui->cbxStopper->setCurrentText( QString::number( model->Stopper() ) );
    ui->cbxStopper->setEnabled( true );

    ui->chxPrintLo->setChecked( model->IsPrintLo() );
}

void HxModelWindow::ShowMarkPositions()
{
    QSignalBlocker blocker( ui->widPosition );
    auto& model = m_pModel;
    if ( !model )
        model = ModelManager()->Create();

    auto& positions = model->Positions();
    int rows = s_maxPointSupported;
    ui->tbvMarkPos->setRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        auto it = positions.find( row + 1 );
        if ( it == positions.end() )
        {
            ui->tbvMarkPos->setText( row, "X", "" );
            ui->tbvMarkPos->setText( row, "Y", "" );
            ui->tbvMarkPos->setText( row, "Góc", "" );
        }
        else
        {
            ui->tbvMarkPos->setText( row, "X", QString::number( it->second.x, 'f', 1 ) );
            ui->tbvMarkPos->setText( row, "Y", QString::number( it->second.y, 'f', 1 ) );
            ui->tbvMarkPos->setText( row, "Góc", QString::number( it->second.angle ) );
        }
    }
}

void HxModelWindow::ShowMarkBlocks()
{
    auto& model = m_pModel;
    if ( !model )
        model = ModelManager()->Create();

    QString designName = model->Design();
    auto design = DesignManager()->GetDesign( designName );
    if ( design == nullptr || design->Blocks().size() == 0 )
    {
        ui->tbvBlocks->setRowCount( 0 );
    }
    else
    {
        int rows = design->Blocks().size();
        ui->tbvBlocks->setRowCount( rows );
        int row = 0;
        for ( auto& [index, block] : design->Blocks() )
        {
            ui->tbvBlocks->setText( row, "Block", QString::number( index ).rightJustified( 3, '0' ) );
            ui->tbvBlocks->setText( row, "Mã", "" );
            ui->tbvBlocks->item( row, "Mã" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
            ui->tbvBlocks->setText( row, "Giá trị", block.data );
            ui->tbvBlocks->setText( row, "Độ dài", QString::number( block.textLen ) );
            row++;
        }
    }
}

void HxModelWindow::ShowComments()
{
    disconnect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
    ui->tbvComments->setRowCount( 0 );
    if ( !m_pModel )
    {
        connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
        return;
    }
    auto& comments = m_pModel->Comments();

    int rows = comments.size();
    ui->tbvComments->setRowCount( rows );

    int row = 0;
    for ( auto& [key, value] : comments )
    {
        ui->tbvComments->setText( row, "Thông tin", key );
        ui->tbvComments->item( row, 0 )->setFlags( ui->tbvComments->item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvComments->setText( row, "Giá trị", value );
        row++;
    }
    connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
}

void HxModelWindow::OnRefresh()
{
    m_models = ModelManager()->GetModels();
    OnFilter( m_pSearchTextBox->text() );
}

void HxModelWindow::OnFilter( const QString& filter = QString() )
{
    QString input = filter.trimmed().toUpper();
    HxModelPtrArray items;
    for ( auto& [code, pModel] : m_models )
    {
        bool bIsMatch = pModel->Code().contains( input ) || pModel->Name().contains( input );

        if ( bIsMatch )
            items.push_back( pModel );
    }

    ui->tbvModels->setRowCount( items.size() );
    int row = 0;
    for ( auto& pModel : items )
    {
        ui->tbvModels->setText( row, "Mã", pModel->Code() );
        ui->tbvModels->setText( row, "Tên", pModel->Name() );
        ui->tbvModels->setText( row, ".No", pModel->kNo() );
        ui->tbvModels->setText( row, "Băng chuyền", QString::number( pModel->CvWidth() ) );
        ui->tbvModels->setText( row, "Vị trí in", QString::number( pModel->Positions().size() ) );
        ui->tbvModels->setText( row, "Chương trình IV", pModel->IVProgram() );
        row++;
    }
}

void HxModelWindow::OnSelect( const QModelIndex& index )
{
    int row = index.row();
    if ( signalsBlocked() || row < 0 )
        return;

    QString modelCode = ui->tbvModels->item( row, 0 )->text();
    m_pModel.reset();
    auto it = m_modelToSave.find( modelCode );
    if ( it != m_modelToSave.end() )
        m_pModel = it->second;

    if ( !m_pModel )
        m_pModel = ModelManager()->GetModel( modelCode );

    ShowModelInfo();
    ShowMarkPositions();
    ShowComments();
    ShowMarkBlocks();
}

void HxModelWindow::OnInfoChanged()
{
    if ( !m_pModel || ui->widInfo->signalsBlocked() )
        return;

    m_pModel->SetCode( ui->txtModelCode->text().trimmed() );
    m_pModel->SetName( ui->txtModelName->text().trimmed() );
    m_pModel->SetPrintLo( ui->chxPrintLo->isChecked() );
    m_pModel->SetkNo( ui->txtModelNo->text().trimmed() );
    m_pModel->SetStopper( ui->cbxStopper->currentIndex() + 1 );
    m_pModel->SetDesign( ui->spxProgram->value() );
    m_pModel->SetIVProgram( ui->cbxIVPrograms->currentText() );
    m_pModel->SetCvWidth( ui->spxCvWidth->value() );

    if ( m_pModel->IsMofified() )
    {
        m_modelToSave[ m_pModel->Code() ] = m_pModel;
    }
}

void HxModelWindow::OnPositionChanged( QStandardItem* item )
{
    if ( !m_pModel || ui->widPosition->signalsBlocked() )
        return;

    int row = item->row();
    int index = row + 1;
    HxDataTable* tableModel = ui->tbvMarkPos->dataTable();
    QString text1 = tableModel->item( row, 0 )->text().trimmed();
    QString text2 = tableModel->item( row, 1 )->text().trimmed();
    QString text3 = tableModel->item( row, 2 )->text().trimmed();

    if ( text1.isEmpty() && text2.isEmpty() && text3.isEmpty() )
    {
        m_pModel->RemovePosition( index );
        m_modelToSave[ m_pModel->Code()] = m_pModel;
        ShowMarkPositions();
    }
    else
    {
        bool bIsVaild[ 3 ] = { false, false, false };
        HxPosition position = m_pModel->Position( index );
        position.x = text1.toDouble( &bIsVaild[ 0 ] );
        position.y = text2.toDouble( &bIsVaild[ 1 ] );
        position.angle = text3.toInt( &bIsVaild[ 2 ] );
        if ( bIsVaild[ 0 ] && bIsVaild[ 1 ] && bIsVaild[ 2 ] )
        {
            position.angle = position.angle / 90 * 90;
            m_pModel->SetPosition( index, position );
            m_modelToSave[ m_pModel->Code() ] = m_pModel;
            ShowMarkPositions();
        }
    }
}

void HxModelWindow::OnCommentChanged( QStandardItem* item )
{
    if ( !m_pModel || ui->widParams->signalsBlocked() )
    {
        // showComments();
        return;
    }

    int row = item->row();
    QString key = ui->tbvComments->item( row, 0 )->text().trimmed().toUpper();
    QString value = ui->tbvComments->item( row, 1 )->text().trimmed().toUpper();
    m_pModel->SetComment( key, value );
    // qDebug() << "CALL 2";
    // showComments();
}

void HxModelWindow::OnNew()
{
    auto dialog = HxNewModelDialog( m_models, m_modelToSave, this );
    if ( dialog.exec() )
    {
        m_pModel = dialog.GetModel();
        m_models[ m_pModel->Code() ] = m_pModel;
        m_modelToSave[ m_pModel->Code() ] = m_pModel;
        ShowModels();
        ShowModelInfo();
        ShowMarkPositions();
        ShowComments();
        ShowMarkBlocks();
        m_pSearchTextBox->setText( m_pModel->Name() );
        OnFilter();
    }
}

void HxModelWindow::OnRemove()
{
    QStringList codes;
    if ( m_pModel )
        codes.push_back( m_pModel->Name() );

    for ( auto& it : ui->tbvModels->selectionModel()->selectedRows() )
    {
        QString code = ui->tbvModels->item( it.row(), 0 )->text();
        if ( !codes.contains( code ) )
            codes.push_back( code );
    }

    if ( codes.isEmpty() )
        return;

    ModelManager()->Removes( codes );
    for ( auto &code : codes )
    {
        m_models.erase( code );
        m_modelToSave.erase( code );
    }

    m_pModel.reset();

    OnRefresh();
    ShowModels();
    ShowModelInfo();
    ShowMarkPositions();
    ShowComments();
    ShowMarkBlocks();
}

void HxModelWindow::OnSave()
{
    OnInfoChanged();
    if ( m_modelToSave.empty() )
        return;

    if ( HxMsgQuestion( tr( "Lưu thông tin thay đổi của %1 model?" ).arg( m_modelToSave.size() ), tr( "Lưu dữ liệu" ) ) == HxMsgButton::Yes )
    {
        for ( auto& [code, pModel] : m_modelToSave )
            ModelManager()->Save( pModel );
        m_modelToSave.clear();
        OnRefresh();
    }
}

void HxModelWindow::OnAddParam()
{
    HxAddParamsDialog dlg( this );
    if ( dlg.exec() )
    {
        QStringList items = dlg.m_names;
        bool bIsApplyALL = dlg.m_bIsApplyAll;

        if ( m_pModel )
            m_pModel->AddComments( items );
        if ( bIsApplyALL )
            ModelManager()->AddComments( items );

        ShowComments();
    }
}


void HxModelWindow::OnRemoveParam()
{
    QStringList names;
    for ( auto& it : ui->tbvComments->selectionModel()->selectedRows() )
    {
        QString key = ui->tbvComments->item( it.row(), 0 )->text();
        names.push_back( key );
    }

    HxRemoveParamDialog dlg( this );
    dlg.setParams( names );
    if ( dlg.exec() )
    {
        names = dlg.names;
        bool bIsApplyALL = dlg.isApplyAll;
        if ( m_pModel )
            m_pModel->RemoveComments( names );
        if ( bIsApplyALL )
            ModelManager()->RemoveComments( names );

        ShowComments();
    }
}


