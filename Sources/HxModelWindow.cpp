#include "HxModelWindow.h"
#include "ui_hxmodelwindow.h"
#include <QSpacerItem>
#include <QFileDialog>

#include "HxAddParamDialog.h"
#include "HxNewModelDialog.h"

#include "HxIVProgram.h"
#include "HxDesign.h"
#include "HxRemoveParamDialog.h"

#include "HxMessage.h"



HxModelWindow::HxModelWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::ModelWindow )
{
    ui->setupUi( this );
    // search
    ui->tbrSearch->addWidget( new QLabel( "Tìm kiếm nhanh " ) );
    txtSearch = new QLineEdit( this );
    txtSearch->setMaximumWidth( 300 );
    ui->tbrSearch->addWidget( txtSearch );
    connect( txtSearch, &QLineEdit::textChanged, this, &HxModelWindow::ModelFilterTextChanged );

    ModelFilterTextChanged( "" );
}

HxModelWindow::~HxModelWindow()
{
    delete ui;
}

void HxModelWindow::showEvent( QShowEvent* )
{
    ModelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::ShowModels()
{
    disconnect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::ModelSelectionChanged );
    int cnt = filtereds.size();
    if ( ui->tbvModels->headers.size() < 1 )
    {
        ui->tbvModels->SetHeaders( { "Mã","Tên",".No","Băng chuyền","Vị trí in","Chương trình IV" } );
        ui->tbvModels->setColumnWidth( 1, 230 );
    }
    ui->tbvModels->SetRowCount( cnt );
    for ( int row = 0; row < cnt; row++ )
    {
        ui->tbvModels->SetText( row, "Mã", filtereds[ row ]->code );
        ui->tbvModels->SetText( row, "Tên", filtereds[ row ]->name );
        ui->tbvModels->SetText( row, ".No", filtereds[ row ]->kNo );
        ui->tbvModels->SetText( row, "Băng chuyền", QString::number( filtereds[ row ]->cvWidth ) );
        ui->tbvModels->SetText( row, "Vị trí in", QString::number( filtereds[ row ]->positions.size() ) );
        ui->tbvModels->SetText( row, "Chương trình IV", filtereds[ row ]->ivProgram );
    }
    connect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::ModelSelectionChanged );
}

void HxModelWindow::ShowModelInfo()
{
    auto model = selected;
    if ( model == nullptr ) model = std::make_shared<HxModel>( HxModel() );
    ui->txtModelName->setText( model->name );
    ui->txtModelCode->setText( model->code );
    ui->txtModelNo->setText( model->kNo );

    // iv
    ui->cbxIVPrograms->setEnabled( false );
    ui->cbxIVPrograms->clear();
    ui->cbxIVPrograms->addItem( "" );
    ui->cbxIVPrograms->addItems( HxIVProgram::Names() );
    ui->cbxIVPrograms->setCurrentText( model->ivProgram );
    ui->cbxIVPrograms->setEnabled( true );

    // design
    ui->spxProgram->setEnabled( false );
    ui->spxProgram->setValue( model->design.toInt( 0 ) );
    ui->spxProgram->setEnabled( true );

    ui->spxCvWidth->setValue( model->cvWidth );

    ui->cbxStopper->setEnabled( false );
    ui->cbxStopper->setCurrentText( QString::number( model->stopper ) );
    ui->cbxStopper->setEnabled( true );
}

void HxModelWindow::ShowMarkPositions()
{
    if ( ui->tbvMarkPos->headers.size() < 1 )
    {
        ui->tbvMarkPos->SetHeaders( { "X","Y","Góc" } );
    }

    auto model = selected;
    if ( model == nullptr ) model = std::make_shared<HxModel>( HxModel() );

    disconnect( ui->tbvMarkPos->DataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::MarkPositionChanged );

    int rows = 60;
    ui->tbvMarkPos->SetRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        if ( model == nullptr || row >= model->positions.size() )
        {
            ui->tbvMarkPos->SetText( row, "X", "" );
            ui->tbvMarkPos->SetText( row, "Y", "" );
            ui->tbvMarkPos->SetText( row, "Góc", "" );
        }
        else
        {
            ui->tbvMarkPos->SetText( row, "X", QString::number( model->positions[ row ].x, 'f', 1 ) );
            ui->tbvMarkPos->SetText( row, "Y", QString::number( model->positions[ row ].y, 'f', 1 ) );
            ui->tbvMarkPos->SetText( row, "Góc", QString::number( model->positions[ row ].angle ) );
        }
    }
    connect( ui->tbvMarkPos->DataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::MarkPositionChanged );
}

void HxModelWindow::ShowMarkBlocks()
{
    if ( ui->tbvBlocks->headers.size() < 1 )
    {
        ui->tbvBlocks->SetHeaders( { "Block","Mã","Độ dài","Giá trị" } );
        ui->tbvBlocks->setColumnWidth( 0, 60 );
        ui->tbvBlocks->setColumnWidth( 1, 60 );
        ui->tbvBlocks->setColumnWidth( 2, 60 );
    }
    auto model = selected;
    if ( model == nullptr ) model = std::make_shared<HxModel>( HxModel() );

    QString designName = model->design;
    auto design = HxDesign::Find( designName );
    if ( design == nullptr || design->blocks.size() == 0 )
    {
        ui->tbvBlocks->SetRowCount( 0 );
    }
    else
    {
        QList<int> nums = design->blocks.keys();
        int rows = nums.size();
        ui->tbvBlocks->SetRowCount( rows );
        for ( int row = 0; row < rows; row++ )
        {
            HxBlock block = design->blocks[ nums[ row ] ];
            ui->tbvBlocks->SetText( row, "Block", QString::number( nums[ row ] ).rightJustified( 3, '0' ) );
            ui->tbvBlocks->SetText( row, "Mã", "" );
            ui->tbvBlocks->Item( row, "Mã" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
            ui->tbvBlocks->SetText( row, "Giá trị", block.data );
            ui->tbvBlocks->SetText( row, "Độ dài", QString::number( block.textLen ) );
        }
    }
}

void HxModelWindow::ShowComments()
{
    if ( ui->tbvComments->headers.size() < 1 )
    {
        ui->tbvComments->SetHeaders( { "Thông tin","Giá trị" } );
    }

    HxDataTable* tableModel = ui->tbvComments->DataTable();
    disconnect( tableModel, &QStandardItemModel::itemChanged, this, &HxModelWindow::CommentChanged );

    ui->tbvComments->SetRowCount( 0 );
    if ( selected == nullptr ) return;

    QStringList keys = selected->comments.keys();

    int rows = keys.size();
    ui->tbvComments->SetRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvComments->SetText( row, "Thông tin", keys[ row ] );
        ui->tbvComments->Item( row, 0 )->setFlags( ui->tbvComments->Item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvComments->SetText( row, "Giá trị", selected->comments.value( keys[ row ] ) );
    }

    connect( tableModel, &QStandardItemModel::itemChanged, this, &HxModelWindow::CommentChanged );
}

void HxModelWindow::ModelFilterTextChanged( QString filter )
{
    filter = filter.trimmed().toLower();
    filtereds.clear();
    for ( auto& model : HxModel::items )
    {
        if ( filter.length() < 1 )
        {
            filtereds.push_back( model );
            continue;
        }

        if ( model->code.toLower().contains( filter ) )
        {
            filtereds.push_back( model );
            continue;
        }
        if ( model->name.toLower().contains( filter ) )
        {
            filtereds.push_back( model );
            continue;
        }
        if ( model->ivProgram.toLower().contains( filter ) )
        {
            filtereds.push_back( model );
            continue;
        }
    }
    ShowModels();
}

void HxModelWindow::ModelSelectionChanged( const QModelIndex& index )
{
    selected.reset();
    int row = index.row();
    selected = filtereds[ row ];
    ShowModelInfo();
    ShowMarkPositions();
    ShowMarkBlocks();
    ShowComments();
    HxModel::Save( selected );
}

void HxModelWindow::MarkPositionChanged( QStandardItem* item )
{
    if ( selected == nullptr ) return;
    HxDataTable* tableModel = ui->tbvMarkPos->DataTable();
    int row = item->row();
    QString text1 = tableModel->item( row, 0 )->text().trimmed();
    QString text2 = tableModel->item( row, 1 )->text().trimmed();
    QString text3 = tableModel->item( row, 2 )->text().trimmed();

    bool changed = false;
    if ( text1 == "" && text2 == "" && text3 == "" )
    {
        if ( row < selected->positions.size() )
        {
            selected->positions.erase( selected->positions.begin() + row );
            changed = true;
        }
    }
    else if ( text1 != "" && text2 != "" && text3 != "" )
    {
        HxPosition pos;
        pos.x = text1.toDouble();
        pos.y = text2.toDouble();
        pos.angle = text3.toDouble( 0 );
        pos.angle = pos.angle % 360;
        pos.angle = pos.angle / 90 * 90;
        if ( row < selected->positions.size() )
        {
            selected->positions[ row ] = pos;
        }
        else
        {
            selected->positions.push_back( pos );
        }
        changed = true;
    }
    if ( changed )
    {
        ShowMarkPositions();
        HxModel::Save( selected );
    }
}

void HxModelWindow::CommentChanged( QStandardItem* item )
{
    if ( selected == nullptr )
    {
        ShowComments();
        return;
    }
    int row = item->row();
    QString key = ui->tbvComments->Item( row, 0 )->text().trimmed().toUpper();
    QString value = ui->tbvComments->Item( row, 1 )->text().trimmed().toUpper();
    selected->comments[ key ] = value;
    ShowComments();
    HxModel::Save( selected );
}

void HxModelWindow::on_actionNew_triggered()
{
    auto dialog = HxNewModelDialog( this );
    if ( dialog.exec() )
    {
        selected.reset();
        ModelFilterTextChanged( "" );

    }
}

void HxModelWindow::on_actionRemove_triggered()
{
    std::vector<int> indexs;
    for ( auto it : ui->tbvModels->selectionModel()->selectedRows() )
    {
        indexs.push_back( it.row() );
    }
    std::sort( indexs.begin(), indexs.end(), []( int i1, int i2 ) { return i1 > i2; } );

    if ( indexs.size() <= 0 ) return;
    int ret = HxMessage::warning( "Xóa các model đang chọn ?", "Khoan đã" );
    if ( ret == QMessageBox::Yes )
    {
        for ( auto index : indexs )
        {
            HxModel::items.erase( HxModel::items.begin() + index );
        }
    }
    HxModel::Save();
    ModelFilterTextChanged( "" );
    ShowModels();
}

void HxModelWindow::on_actionSave_triggered()
{
    HxModel::Save();
}

void HxModelWindow::on_actionLoad_triggered()
{

}

void HxModelWindow::on_actionFromFile_triggered()
{
    QFileDialog fileDialog( this );
    if ( fileDialog.exec() )
    {
        QStringList files = fileDialog.selectedFiles();
        HxModel::AddModelsFromFile( files[ 0 ] );
        ModelFilterTextChanged( "" );
    }
}

void HxModelWindow::on_actionToFile_triggered()
{

}

void HxModelWindow::on_txtModelNo_textChanged( const QString& arg1 )
{
    if ( selected == nullptr )
    {
        return;
    }
    selected->kNo = arg1.trimmed().toUpper();
    HxModel::Save( selected );
    ModelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::on_spxProgram_valueChanged( int arg1 )
{
    if ( ui->spxProgram->isEnabled() == false ) return;
    if ( selected == nullptr )
    {
        return;
    }
    selected->design = QString::number( arg1 ).rightJustified( 4, '0' );
    ShowMarkBlocks();
    HxModel::Save( selected );
    ModelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::on_spxCvWidth_valueChanged( double arg1 )
{
    if ( selected == nullptr )
    {
        return;
    }
    selected->cvWidth = arg1;
    HxModel::Save( selected );
    ModelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::on_btnAddParam_clicked()
{
    HxAddParamDialog dlg( this );
    if ( dlg.exec() )
    {
        QStringList items = dlg.names;
        bool applyAll = dlg.isApplyAll;
        if ( applyAll )
        {
            for ( auto& md : HxModel::items )
            {
                for ( auto& it : items )
                {
                    if ( md->comments.contains( it ) == false )
                    {
                        md->comments.insert( it, "" );
                    }
                }
            }
        }
        else if ( selected != nullptr )
        {
            for ( auto& it : items )
            {
                if ( selected->comments.contains( it ) == false )
                {
                    selected->comments.insert( it, "" );
                }
            }
        }
        ShowComments();
        HxModel::Save( selected );
    }
}


void HxModelWindow::on_btnRemoveParam_clicked()
{
    QStringList names;
    for ( auto it : ui->tbvComments->selectionModel()->selectedRows() )
    {
        QString key = ui->tbvComments->Item( it.row(), 0 )->text();
        names.push_back( key );
    }

    HxRemoveParamDialog dlg( this );
    dlg.SetParams( names );
    if ( dlg.exec() )
    {
        bool applyAll = dlg.isApplyAll;
        if ( applyAll )
        {
            for ( auto& md : HxModel::items )
            {
                for ( auto& it : names )
                {
                    if ( md->comments.contains( it ) )
                    {
                        md->comments.remove( it );
                    }
                }
            }
        }
        else if ( selected != nullptr )
        {
            for ( auto& it : names )
            {
                if ( selected->comments.contains( it ) )
                {
                    selected->comments.remove( it );
                }
            }
        }
        ShowComments();
        HxModel::Save( selected );
    }
}


void HxModelWindow::on_cbxStopper_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxStopper->isEnabled() == false ) return;
    if ( selected == nullptr ) return;
    selected->stopper = arg1.toInt();
    HxModel::Save( selected );
    ModelFilterTextChanged( txtSearch->text() );
}


void HxModelWindow::on_cbxIVPrograms_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxIVPrograms->isEnabled() == false ) return;
    if ( selected == nullptr ) return;
    selected->ivProgram = arg1;
    HxModel::Save( selected );
    ModelFilterTextChanged( txtSearch->text() );
}




