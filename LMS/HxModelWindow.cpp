#include "HxModelWindow.h"
#include "ui_HxModelWindow.h"
#include <QSpacerItem>
#include <QFileDialog>

#include "QSignalBlocker"

#include "HxAddParamsDialog.h"
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

    ui->cbxIVPrograms->addItem( "" );
    ui->cbxIVPrograms->addItems( HxIVProgram::names() );

    connect(ui->actionNew, &QAction::triggered, this, &HxModelWindow::OnNew);
    connect(ui->actionRemove, &QAction::triggered, this, &HxModelWindow::OnRemove);
    connect(ui->actionSave, &QAction::triggered, this, &HxModelWindow::OnSave);
    connect(ui->actionLoad, &QAction::triggered, this, &HxModelWindow::OnRefresh);
    connect( txtSearch, &QLineEdit::textChanged, this, &HxModelWindow::OnFilter );
    connect( txtSearch, &QLineEdit::editingFinished, this, &HxModelWindow::OnFilter );

    ui->tbvModels->setHeaders( { "Mã","Tên",".No","Băng chuyền","Vị trí in","Chương trình IV" } );
    ui->tbvModels->setColumnWidth( 1, 230 );

    ui->tbvMarkPos->setHeaders( { "X","Y","Góc" } );
    ui->tbvComments->setHeaders( { "Thông tin","Giá trị" } );
    ui->tbvBlocks->setHeaders( { "Block","Mã","Độ dài","Giá trị" } );
    ui->tbvBlocks->setColumnWidth( 0, 60 );
    ui->tbvBlocks->setColumnWidth( 1, 60 );
    ui->tbvBlocks->setColumnWidth( 2, 60 );




    OnRefresh();

    connect(ui->txtModelCode, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged);
    connect(ui->txtModelName, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged);
    connect(ui->txtModelNo, &QLineEdit::editingFinished, this, &HxModelWindow::OnInfoChanged);
    connect(ui->cbxStopper, &QComboBox::currentIndexChanged, this, &HxModelWindow::OnInfoChanged);
    connect(ui->spxProgram, &QSpinBox::editingFinished, this, &HxModelWindow::OnInfoChanged);
    connect(ui->cbxIVPrograms, &QComboBox::currentIndexChanged, this, &HxModelWindow::OnInfoChanged);
    connect(ui->spxCvWidth, &QDoubleSpinBox::editingFinished, this, &HxModelWindow::OnInfoChanged);
    connect(ui->chxPrintLo, &QCheckBox::toggled, this, &HxModelWindow::OnInfoChanged);

    connect( ui->tbvModels, &HxTableView::pressed, this, &HxModelWindow::OnSelectModel );
    // connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
    connect( ui->tbvMarkPos->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnPositionChanged );

}

HxModelWindow::~HxModelWindow()
{
    delete ui;
}

void HxModelWindow::showEvent( QShowEvent* )
{
    OnRefresh();
}

void HxModelWindow::showModels()
{
    QSignalBlocker blocker(this);
    int rows = m_models.size();
    ui->tbvModels->setRowCount(rows);
    int row = 0;
    for(auto &[name, pModel]: m_models)
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

void HxModelWindow::showModelInfo()
{
    QSignalBlocker bocker(this);
    auto model = m_pModel;
    if ( !model )
        model = GetModelManager()->Create();
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

    ui->chxPrintLo->setChecked( model->IsPrintLo());
}

void HxModelWindow::showMarkPositions()
{
    QSignalBlocker blocker(this);
    auto model = m_pModel;
    if ( !model )
        model = GetModelManager()->Create();

    auto positions = model->Positions();
    int rows = 60;
    ui->tbvMarkPos->setRowCount( rows );

    for(int row = 0; row < rows; row ++)
    {
        auto it = positions.find(row+1);
        if(it == positions.end())
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

void HxModelWindow::showMarkBlocks()
{
    auto model = m_pModel;
    if ( !model )
        model = GetModelManager()->Create();

    QString designName = model->Design();
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
            HxBlockInfo block = design->blocks[ nums[ row ] ];
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
    disconnect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
    ui->tbvComments->setRowCount( 0 );
    if ( !m_pModel)
    {
        connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
        return;
    }
    auto comments = m_pModel->Comments();

    int rows = comments.size();
    ui->tbvComments->setRowCount( rows );

    int row = 0;
    for( auto& [key, value]: comments)
    {
        ui->tbvComments->setText( row, "Thông tin", key );
        ui->tbvComments->item( row, 0 )->setFlags( ui->tbvComments->item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvComments->setText( row, "Giá trị", value );
        row++;
    }
    connect( ui->tbvComments->dataTable(), &QStandardItemModel::itemChanged, this, &HxModelWindow::OnCommentChanged );
}

void HxModelWindow::OnFilter()
{
    QString filter = txtSearch->text().trimmed().toLower();
    if(filter.isEmpty())
        return;
    QStringList columns = {"Mã","Tên","Chương trình IV"};
    int rows = ui->tbvModels->RowCount();
    for(int row = 0; row < rows; row++)
    {
        bool bIsFound = false;
        for(auto &column : columns)
        {
            QString text = ui->tbvModels->item(row, column)->text().toLower();
            bIsFound |= text.contains(filter);
            if( bIsFound )
            {
                ui->tbvModels->scrollTo(ui->tbvModels->model()->index(row,0),QTableView::ScrollHint::PositionAtTop);
                break;
            }
        }
        if(bIsFound)
            break;
    }
}

void HxModelWindow::OnSelectModel( const QModelIndex& index )
{
    if(signalsBlocked())
        return;

    if(m_pModel !=nullptr && m_pModel->IsMofified())
        OnSave();

    m_pModel.reset();
    int row = index.row();
    if( row >= 0 )
    {
        QString modelName = ui->tbvModels->item(row,1)->text();
        m_pModel = GetModelManager()->GetModel(modelName);
    }

    showModelInfo();
    showMarkPositions();
    showComments();
    showMarkBlocks();
}

void HxModelWindow::OnPositionChanged( QStandardItem* item )
{
    if ( !m_pModel || signalsBlocked() )
        return;

    int row = item->row();
    int index = row + 1;
    HxDataTable* tableModel = ui->tbvMarkPos->dataTable();
    QString text1 = tableModel->item(row,0)->text().trimmed();
    QString text2 = tableModel->item(row,1)->text().trimmed();
    QString text3 = tableModel->item(row,2)->text().trimmed();

    if(text1.isEmpty() && text2.isEmpty() && text3.isEmpty())
    {
        m_pModel->RemovePosition( index );
        showMarkPositions();
    }
    else
    {
        bool bIsVaild[3] = {false, false, false};
        HxPosition position = m_pModel->Position( index );
        position.x = text1.toDouble(&bIsVaild[0]);
        position.y = text2.toDouble(&bIsVaild[1]);
        position.angle = text3.toInt(&bIsVaild[2]);
        if(bIsVaild[0] && bIsVaild[1] && bIsVaild[2])
        {
            position.angle = position.angle / 90 * 90;
            m_pModel->SetPosition( index, position );
            showMarkPositions();
        }
    }

    // // int row = item->row();
    // // int col = item->column();
    // bool bIsVaild = false;
    // int value = tableModel->item(row, col)->text().trimmed().toInt(&bIsVaild);

    // if(!bIsVaild)
    // {
    //     showMarkPositions();
    //     return;
    // }

    // int index = row + 1;
    // HxPosition position = m_pModel->Position( index );
    // switch (col) {
    // case 0:
    //     position.x = value;
    //     break;
    // case 1:
    //     position.y = value;
    //     break;
    // case 2:
    // {
    //     value = value / 90 * 90;
    //     position.angle = value;
    // }
    // break;
    // default:
    //     break;
    // }
    // m_pModel->SetPosition( index, position );
    // showMarkPositions();
}

void HxModelWindow::OnCommentChanged( QStandardItem* item )
{
    qDebug() << "CALL";
    if ( !m_pModel || signalsBlocked())
    {
        // showComments();
        return;
    }

    int row = item->row();
    QString key = ui->tbvComments->item( row, 0 )->text().trimmed().toUpper();
    QString value = ui->tbvComments->item( row, 1 )->text().trimmed().toUpper();
    m_pModel->SetComment( key, value);
    // qDebug() << "CALL 2";
    // showComments();
}

void HxModelWindow::OnNew()
{
    OnSave();
    auto dialog = HxNewModelDialog( m_models, this );
    if ( dialog.exec() )
    {
        m_pModel = dialog.GetModel();
        m_models[m_pModel->Name()] = m_pModel;
        showModels();
        showModelInfo();
        showMarkPositions();
        showComments();
        showMarkBlocks();
        txtSearch->setText(m_pModel->Name());
        OnFilter();
    }
}

void HxModelWindow::OnRemove()
{
    QStringList names;
    if(m_pModel)
        names.push_back(m_pModel->Name());

    for ( auto &it : ui->tbvModels->selectionModel()->selectedRows() )
    {
        QString name = ui->tbvModels->item( it.row(),1 )->text();
        if(!names.contains(name))
            names.push_back(name);
    }

    if(names.isEmpty())
        return;

    GetModelManager()->Removes( names );
    m_pModel.reset();
    OnRefresh();
    showModels();
    showModelInfo();
    showMarkPositions();
    showComments();
    showMarkBlocks();
}

void HxModelWindow::OnSave()
{
    if(!m_pModel)
        return;

    OnInfoChanged();
    if(HxMsgQuestion(tr("Lưu thông tin thay đổi của model \"%1\"?").arg(m_pModel->Name()),tr("Lưu dữ liệu")) == HxMsgButton::Yes)
    {
        GetModelManager()->Save(m_pModel);
    }
}

void HxModelWindow::OnRefresh()
{
    m_models = GetModelManager()->GetModels();
    showModels();
}

void HxModelWindow::OnInfoChanged()
{
    if(m_pModel == nullptr || signalsBlocked())
        return;

    m_pModel->SetCode(ui->txtModelCode->text().trimmed());
    m_pModel->SetName(ui->txtModelName->text().trimmed());
    m_pModel->SetPrintLo(ui->chxPrintLo->isChecked());
    m_pModel->SetkNo(ui->txtModelNo->text().trimmed());
    m_pModel->SetStopper(ui->cbxStopper->currentIndex() + 1);
    m_pModel->SetDesign(ui->spxProgram->value());
    m_pModel->SetIVProgram(ui->cbxIVPrograms->currentText());
    m_pModel->SetCvWidth(ui->spxCvWidth->value());
}

void HxModelWindow::OnAddParam()
{
    HxAddParamsDialog dlg( this );
    if ( dlg.exec() )
    {
        QStringList items = dlg.m_names;
        bool bIsApplyALL = dlg.m_bIsApplyAll;

        if(m_pModel)
            m_pModel->AddComments(items);
        if(bIsApplyALL)
            GetModelManager()->AddComments(items);

        showComments();
    }
}


void HxModelWindow::OnRemoveParam()
{
    QStringList names;
    for ( auto &it : ui->tbvComments->selectionModel()->selectedRows() )
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
        if(m_pModel)
            m_pModel->RemoveComments(names);
        if(bIsApplyALL)
        GetModelManager()->RemoveComments(names);

        showComments();
    }
}


