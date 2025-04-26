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
    connect( txtSearch, &QLineEdit::textChanged, this, &HxModelWindow::modelFilterTextChanged );

    modelFilterTextChanged( "" );
}

HxModelWindow::~HxModelWindow()
{
    delete ui;
}

void HxModelWindow::showEvent( QShowEvent* )
{
    modelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::showModels()
{
    disconnect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::modelSelectionChanged );
    int cnt = filtereds.size();
    if ( ui->tbvModels->headers.size() < 1 )
    {
        ui->tbvModels->setHeaders( { "Mã","Tên",".No","Băng chuyền","Vị trí in","Chương trình IV" } );
        ui->tbvModels->setColumnWidth( 1, 230 );
    }
    ui->tbvModels->setRowCount( cnt );
    for ( int row = 0; row < cnt; row++ )
    {
        ui->tbvModels->setText( row, "Mã", filtereds[ row ]->code );
        ui->tbvModels->setText( row, "Tên", filtereds[ row ]->name );
        ui->tbvModels->setText( row, ".No", filtereds[ row ]->kNo );
        ui->tbvModels->setText( row, "Băng chuyền", QString::number( filtereds[ row ]->cvWidth ) );
        ui->tbvModels->setText( row, "Vị trí in", QString::number( filtereds[ row ]->positions.size() ) );
        ui->tbvModels->setText( row, "Chương trình IV", filtereds[ row ]->ivProgram );
    }
    connect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::modelSelectionChanged );
}

void HxModelWindow::showModelInfo()
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
    ui->cbxIVPrograms->addItems( HxIVProgram::names() );
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

void HxModelWindow::showMarkPositions()
{
    if ( ui->tbvMarkPos->headers.size() < 1 )
    {
        ui->tbvMarkPos->setHeaders( { "X","Y","Góc" } );
    }

    auto model = selected;
    if ( model == nullptr ) model = std::make_shared<HxModel>( HxModel() );

    disconnect( ui->tbvMarkPos->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::markPositionChanged );

    int rows = 60;
    ui->tbvMarkPos->setRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        if ( model == nullptr || row >= model->positions.size() )
        {
            ui->tbvMarkPos->setText( row, "X", "" );
            ui->tbvMarkPos->setText( row, "Y", "" );
            ui->tbvMarkPos->setText( row, "Góc", "" );
        }
        else
        {
            ui->tbvMarkPos->setText( row, "X", QString::number( model->positions[ row ].x, 'f', 1 ) );
            ui->tbvMarkPos->setText( row, "Y", QString::number( model->positions[ row ].y, 'f', 1 ) );
            ui->tbvMarkPos->setText( row, "Góc", QString::number( model->positions[ row ].angle ) );
        }
    }
    connect( ui->tbvMarkPos->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::markPositionChanged );
}

void HxModelWindow::showMarkBlocks()
{
    if ( ui->tbvBlocks->headers.size() < 1 )
    {
        ui->tbvBlocks->setHeaders( { "Block","Mã","Độ dài","Giá trị" } );
        ui->tbvBlocks->setColumnWidth( 0, 60 );
        ui->tbvBlocks->setColumnWidth( 1, 60 );
        ui->tbvBlocks->setColumnWidth( 2, 60 );
    }
    auto model = selected;
    if ( model == nullptr ) model = std::make_shared<HxModel>( HxModel() );

    QString designName = model->design;
    auto design = HxDesign::find( designName );
    if ( design == nullptr || design->blocks.size() == 0 )
    {
        ui->tbvBlocks->setRowCount( 0 );
    }
    else
    {
        QList<int> nums = design->blocks.keys();
        int rows = nums.size();
        ui->tbvBlocks->setRowCount( rows );
        for ( int row = 0; row < rows; row++ )
        {
            HxBlock block = design->blocks[ nums[ row ] ];
            ui->tbvBlocks->setText( row, "Block", QString::number( nums[ row ] ).rightJustified( 3, '0' ) );
            ui->tbvBlocks->setText( row, "Mã", "" );
            ui->tbvBlocks->item( row, "Mã" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
            ui->tbvBlocks->setText( row, "Giá trị", block.data );
            ui->tbvBlocks->setText( row, "Độ dài", QString::number( block.textLen ) );
        }
    }
}

void HxModelWindow::showComments()
{
    if ( ui->tbvComments->headers.size() < 1 )
    {
        ui->tbvComments->setHeaders( { "Thông tin","Giá trị" } );
    }

    HxDataTable* tableModel = ui->tbvComments->dataTable();
    disconnect( tableModel, &QStandardItemModel::itemChanged, this, &HxModelWindow::commentChanged );

    ui->tbvComments->setRowCount( 0 );
    if ( selected == nullptr ) return;

    QStringList keys = selected->comments.keys();

    int rows = keys.size();
    ui->tbvComments->setRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvComments->setText( row, "Thông tin", keys[ row ] );
        ui->tbvComments->item( row, 0 )->setFlags( ui->tbvComments->item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvComments->setText( row, "Giá trị", selected->comments.value( keys[ row ] ) );
    }

    connect( tableModel, &QStandardItemModel::itemChanged, this, &HxModelWindow::commentChanged );
}

void HxModelWindow::modelFilterTextChanged( QString filter )
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
    showModels();
}

void HxModelWindow::modelSelectionChanged( const QModelIndex& index )
{
    selected.reset();
    int row = index.row();
    selected = filtereds[ row ];
    showModelInfo();
    showMarkPositions();
    showMarkBlocks();
    showComments();
    HxModel::save( selected );
}

void HxModelWindow::markPositionChanged( QStandardItem* item )
{
    if ( selected == nullptr ) return;
    HxDataTable* tableModel = ui->tbvMarkPos->dataTable();
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
        showMarkPositions();
        HxModel::save( selected );
    }
}

void HxModelWindow::commentChanged( QStandardItem* item )
{
    if ( selected == nullptr )
    {
        showComments();
        return;
    }
    int row = item->row();
    QString key = ui->tbvComments->item( row, 0 )->text().trimmed().toUpper();
    QString value = ui->tbvComments->item( row, 1 )->text().trimmed().toUpper();
    selected->comments[ key ] = value;
    showComments();
    HxModel::save( selected );
}

void HxModelWindow::on_actionNew_triggered()
{
    auto dialog = HxNewModelDialog( this );
    if ( dialog.exec() )
    {
        selected.reset();
        modelFilterTextChanged( "" );

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
    HxModel::save();
    modelFilterTextChanged( "" );
    showModels();
}

void HxModelWindow::on_actionSave_triggered()
{
    HxModel::save();
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
        HxModel::addModelsFromFile( files[ 0 ] );
        modelFilterTextChanged( "" );
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
    HxModel::save( selected );
    modelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::on_spxProgram_valueChanged( int arg1 )
{
    if ( ui->spxProgram->isEnabled() == false ) return;
    if ( selected == nullptr )
    {
        return;
    }
    selected->design = QString::number( arg1 ).rightJustified( 4, '0' );
    showMarkBlocks();
    HxModel::save( selected );
    modelFilterTextChanged( txtSearch->text() );
}

void HxModelWindow::on_spxCvWidth_valueChanged( double arg1 )
{
    if ( selected == nullptr )
    {
        return;
    }
    selected->cvWidth = arg1;
    HxModel::save( selected );
    modelFilterTextChanged( txtSearch->text() );
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
        showComments();
        HxModel::save( selected );
    }
}


void HxModelWindow::on_btnRemoveParam_clicked()
{
    QStringList names;
    for ( auto it : ui->tbvComments->selectionModel()->selectedRows() )
    {
        QString key = ui->tbvComments->item( it.row(), 0 )->text();
        names.push_back( key );
    }

    HxRemoveParamDialog dlg( this );
    dlg.setParams( names );
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
        showComments();
        HxModel::save( selected );
    }
}


void HxModelWindow::on_cbxStopper_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxStopper->isEnabled() == false ) return;
    if ( selected == nullptr ) return;
    selected->stopper = arg1.toInt();
    HxModel::save( selected );
    modelFilterTextChanged( txtSearch->text() );
}


void HxModelWindow::on_cbxIVPrograms_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxIVPrograms->isEnabled() == false ) return;
    if ( selected == nullptr ) return;
    selected->ivProgram = arg1;
    HxModel::save( selected );
    modelFilterTextChanged( txtSearch->text() );
}




