#include "HxLOTpropertyDialog.h"
#include "ui_hxlotpropertydialog.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"
#include "HxConvert.h"


HxLOTPropertyDialog::HxLOTPropertyDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::LOTPropertyDialog )
{
    ui->setupUi( this );
}

HxLOTPropertyDialog::~HxLOTPropertyDialog()
{
    delete ui;
}

void HxLOTPropertyDialog::PasteToLot( std::shared_ptr<HxLOT> dstLot, bool newLot )
{
    if ( dstLot == nullptr ) return;
    if ( newLot )
    {
        dstLot->progress = 0;
        dstLot->dateCreate = QDateTime::currentDateTime().toString( "yyyy-MM-dd HH:mm:ss" );
        dstLot->isRePrint = ui->chxRePrint->isChecked();
        dstLot->name = ui->txtName->text().trimmed().toUpper();
        if ( dstLot->isRePrint )
        {
            dstLot->name += "--RE" + QDateTime::currentDateTime().toString( "HHmmss" );
        }
    }


    dstLot->modelName = ui->cbxModel->currentText();
    dstLot->macStart = ui->txtMACStart->text();
    dstLot->macEnd = ui->txtMACEnd->text();
    dstLot->quantity = ui->spxQuantity->value();
    dstLot->counterStart = ui->txtCounterStart->text().trimmed();


    // param
    int rows = ui->tbvParams->DataTable()->rowCount();
    for ( int row = 0; row < rows; row++ )
    {
        QString key = ui->tbvParams->Item( row, 0 )->text();
        QString value = ui->tbvParams->Item( row, 1 )->text();
        if ( dstLot->comments.contains( key ) )
        {
            dstLot->comments[ key ] = value;
        }
        else
        {
            dstLot->comments.insert( key, value );
        }
    }
}

void HxLOTPropertyDialog::ShowInfo()
{
    if ( lot != nullptr )
    {
        this->setWindowTitle( "Thông tin lot" );
        ui->chxRePrint->setChecked( lot->isRePrint );
        ui->txtName->setEnabled( false );
        ui->chxRePrint->setEnabled( false );
        ui->txtName->setText( lot->name );
        ui->cbxModel->setCurrentText( lot->modelName );
        ui->txtMACStart->setText( lot->macStart );
        ui->txtCounter->setText( lot->Counter() );
        ui->txtMACEnd->setText( lot->macEnd );
        ui->spxQuantity->setValue( lot->quantity );
        ui->txtCounterStart->setText( lot->counterStart );

        if ( lot->Status() == "Chưa sản xuất" )
        {
            ui->cbxModel->setEnabled( true );
            ui->txtMACStart->setEnabled( true );
            ui->txtCounter->setEnabled( true );
            ui->txtMACEnd->setEnabled( true );
            ui->spxQuantity->setEnabled( true );
            ui->txtCounterStart->setEnabled( true );
        }
        else if ( lot->Status() == "Đang sản xuất" )
        {
            ui->cbxModel->setEnabled( false );
            ui->txtMACStart->setEnabled( false );
            ui->txtCounter->setEnabled( true );
            ui->txtMACEnd->setEnabled( true );
            ui->spxQuantity->setEnabled( true );
            ui->txtCounterStart->setEnabled( false );
        }
        else
        {
            ui->cbxModel->setEnabled( false );
            ui->txtMACStart->setEnabled( false );
            ui->txtCounter->setEnabled( true );
            ui->txtMACEnd->setEnabled( true );
            ui->spxQuantity->setEnabled( true );
            ui->txtCounterStart->setEnabled( false );
        }

        ui->btnCreateOrChange->setText( "Thay đổi" );
    }
    else
    {
        this->setWindowTitle( "Tạo lot" );
        Clear();

        ui->btnCreateOrChange->setText( "Tạo mới" );
    }
}

void HxLOTPropertyDialog::ShowParams()
{
    disconnect( ui->tbvParams->DataTable(), &QStandardItemModel::itemChanged, this, &HxLOTPropertyDialog::ParamChanged );
    if ( ui->tbvParams->headers.empty() )
    {
        ui->tbvParams->SetHeaders( { "Thông tin","Giá trị" } );
    }

    QStringList paramNames = {};
    if ( lot != nullptr )
    {
        QStringList keys = lot->comments.keys();
        for ( auto& key : keys )
        {
            if ( !paramNames.contains( key ) ) paramNames.push_back( key );
        }
    }
    else
    {
        HxLOT lt;
        QStringList keys = lt.comments.keys();
        for ( auto& key : keys )
        {
            if ( !paramNames.contains( key ) ) paramNames.push_back( key );
        }
    }

    for ( auto& lt : HxLOT::items )
    {
        QStringList keys = lt->comments.keys();
        for ( auto& key : keys )
        {
            if ( !paramNames.contains( key ) ) paramNames.push_back( key );
        }
    }

    int rows = paramNames.size();
    ui->tbvParams->SetRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvParams->SetText( row, 0, paramNames[ row ] );
        ui->tbvParams->SetText( row, 1, "" );
        if ( lot != nullptr && lot->comments.contains( paramNames[ row ] ) )
        {
            ui->tbvParams->SetText( row, 1, lot->comments.value( paramNames[ row ] ) );
        }
    }
    connect( ui->tbvParams->DataTable(), &QStandardItemModel::itemChanged, this, &HxLOTPropertyDialog::ParamChanged );
}

void HxLOTPropertyDialog::ShowBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->SetHeaders( { "Block","Nội dung","Độ dài","Định dạng" } );
        ui->tbvBlocks->setColumnWidth( 1, 300 );
    }
    ui->tbvBlocks->SetRowCount( 0 );
    auto tmp = std::make_shared<HxLOT>( HxLOT() );
    PasteToLot( tmp, lot == nullptr );

    auto model = HxModel::Find( tmp->modelName );
    if ( model == nullptr ) return;
    auto design = HxDesign::Find( model->design );
    if ( design == nullptr ) return;
    QList<int> nums = design->blocks.keys();
    std::sort( nums.begin(), nums.end() );

    ui->tbvBlocks->SetRowCount( nums.size() );
    for ( int i = 0; i < nums.size(); i++ )
    {
        int blockIdx = nums[ i ];
        ui->tbvBlocks->SetText( i, "Block", QString::number( nums[ i ] ).rightJustified( 3, '0' ) );

        QString data = HxBlock::gen( design->blocks[ blockIdx ].data, tmp, model );
        ui->tbvBlocks->SetText( i, "Nội dung", data );

        ui->tbvBlocks->SetText( i, "Định dạng", design->blocks[ blockIdx ].data );

        int len = data.length();
        int setupLen = design->blocks[ blockIdx ].textLen;
        bool match = len == setupLen;
        ui->tbvBlocks->SetText( i, "Độ dài", QString::number( len ) + "/" + QString::number( setupLen ) );

        QColor color = QColor( 0, 128, 0 );
        if ( !match ) color = QColor( 128, 0, 0 );
        for ( int col = 0; col < ui->tbvBlocks->DataTable()->columnCount(); col++ )
        {
            ui->tbvBlocks->Item( i, col )->setForeground( color );
        }
    }
}

bool HxLOTPropertyDialog::CheckLotName()
{
    if ( ui->chxRePrint->isChecked() ) return true;
    QString lotName = ui->txtName->text().trimmed().toUpper();
    if ( lotName.length() < 1 ) return false;
    for ( auto& it : HxLOT::items )
    {
        if ( it == lot ) continue;
        if ( it->name.toUpper() == lotName )
        {
            return false;
        }
    }
    return true;
}

bool HxLOTPropertyDialog::CheckSeriRange()
{
    if ( ui->chxRePrint->isChecked() ) return true;
    QString modelName = ui->cbxModel->currentText().trimmed().toUpper();
    auto model = HxModel::Find( modelName );
    if ( model == nullptr ) return false;

    auto design = HxDesign::Find( model->design );
    if ( design == nullptr ) return false;
    int indexBlockCode = design->IndexOfBlockCode();
    if ( indexBlockCode < 1 ) return false;
    HxBlock block = design->blocks[ indexBlockCode ];

    auto fromLot = std::make_shared<HxLOT>( HxLOT() );
    PasteToLot( fromLot, true );
    fromLot->progress = 0;
    QString startSerial = HxBlock::gen( block.data, fromLot, model );
    fromLot->progress = fromLot->quantity - 1;
    QString endSerial = HxBlock::gen( block.data, fromLot, model );

    for ( auto& it : HxLOT::items )
    {
        if ( it == lot ) continue;
        if ( it->modelName != model->name ) continue;
        if ( it->name == fromLot->name ) continue;
        auto toLot = it;

        //        // con mẹ nó, chỗ này không backup progress nên mới xảy ra progess = quantity - 1
        //        // khi tạo hoặc sửa lot
        //        toLot->progress = 0;
        //        QString startSerial2 = BlockInfo::gen(block.data, toLot,model);
        //        toLot->progress = toLot->quantity - 1;
        //        QString endSerial2 = BlockInfo::gen(block.data, toLot, model);

                // phải sửa lại như thế này
        int backupProgress = toLot->progress;
        toLot->progress = 0;
        QString startSerial2 = HxBlock::gen( block.data, toLot, model );
        toLot->progress = toLot->quantity - 1;
        QString endSerial2 = HxBlock::gen( block.data, toLot, model );
        // backup
        toLot->progress = backupProgress;

        QStringList checkItems = { startSerial, endSerial };
        if ( checkItems.contains( startSerial2 ) ) return false;
        if ( checkItems.contains( endSerial2 ) ) return false;
        checkItems.push_back( startSerial2 );
        checkItems.push_back( endSerial2 );
        checkItems.sort();

        if ( startSerial == checkItems[ 0 ] && endSerial == checkItems[ 1 ] ) continue;
        if ( startSerial == checkItems[ 2 ] && endSerial == checkItems[ 3 ] ) continue;
        return false;
    }
    return true;
}

bool HxLOTPropertyDialog::CheckMacs()
{
    QString lotName = ui->txtName->text().trimmed().toUpper();
    QString macStart = ui->txtMACStart->text().trimmed();
    QString macEnd = ui->txtMACEnd->text().trimmed();
    int macStartLen = macStart.length();
    int macEndLen = macEnd.length();
    if ( macStartLen != 0 && macStartLen != 12 ) return false;
    if ( macEndLen != 0 && macEndLen != 12 ) return false;
    if ( macStartLen == 0 ) return true;

    if ( macStart != macEnd )
    {
        QStringList checkItems = { macStart, macEnd };
        checkItems.sort();
        if ( macStart == checkItems[ 1 ] ) return false;
    }


    std::string smac = ( macStart + macEnd ).toStdString();
    for ( int i = 0; i < smac.length(); i++ )
    {
        char c = smac[ i ];
        if ( ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) ) continue;
        return false;
    }

    for ( auto& it : HxLOT::items )
    {
        if ( it == lot ) continue;
        if ( it->isRePrint ) continue;
        if ( it->name == lotName ) continue;
        if ( it->macStart.length() != 12 ) continue;
        if ( it->macEnd.length() != 12 ) continue;

        QStringList checkItems = { macStart, macEnd };
        if ( checkItems.contains( it->macStart ) ) return false;
        if ( checkItems.contains( it->macEnd ) ) return false;
        checkItems.push_back( it->macStart );
        checkItems.push_back( it->macEnd );
        checkItems.sort();

        if ( macStart == checkItems[ 0 ] && macEnd == checkItems[ 1 ] ) continue;
        if ( macStart == checkItems[ 2 ] && macEnd == checkItems[ 3 ] ) continue;
        return false;
    }

    return true;
}

bool HxLOTPropertyDialog::CheckModelInfo()
{
    QString modelName = ui->cbxModel->currentText();
    auto model = HxModel::Find( modelName );
    if ( model == nullptr ) return false;
    auto design = HxDesign::Find( model->design );
    if ( design == nullptr ) return false;
    return true;
}

bool HxLOTPropertyDialog::CheckBlocks()
{
    bool match = true;
    int rows = ui->tbvBlocks->DataTable()->rowCount();
    for ( int row = 0; row < rows; row++ )
    {
        QColor color = ui->tbvBlocks->Item( row, 0 )->foreground().color();
        int r = color.red();
        int g = color.green();
        if ( r > g )
        {
            match = false;
            break;
        }
    }
    return match;
}

void HxLOTPropertyDialog::CheckInputs()
{
    bool available = true;
    QStringList errors;

    if ( !CheckLotName() )
    {
        errors.push_back( "Tên lot trống hoặc bị trùng" );
        available = false;
    }

    if ( !CheckSeriRange() )
    {
        errors.push_back( "Dải seri bị trùng" );
        available = false;
    }

    if ( !CheckMacs() )
    {
        errors.push_back( "Dải mac không hợp lệ hoặc bị trùng" );
        available = false;
    }

    if ( !CheckModelInfo() )
    {
        errors.push_back( "Thông tin model chưa đúng" );
        available = false;
    }

    if ( !CheckBlocks() )
    {
        errors.push_back( "Phát hiện dữ liệu block sai quy cách" );
        available = false;
    }

    ui->lblError->setText( errors.join( "; " ) );
    ui->btnCreateOrChange->setEnabled( available );
}


void HxLOTPropertyDialog::SetData( std::shared_ptr<HxLOT> data )
{
    // model list
    QStringList names = HxModel::Names();
    ui->cbxModel->clear();
    ui->cbxModel->addItem( "" );
    ui->cbxModel->addItems( names );

    lot = data;
    ShowInfo();
    ShowParams();
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::Clear()
{
    lot = nullptr;
    ui->txtName->setText( "" );
    ui->cbxModel->setCurrentText( "" );
    ui->txtMACStart->setText( "" );
    ui->txtMACEnd->setText( "" );
    ui->spxQuantity->setValue( 0 );
}



void HxLOTPropertyDialog::on_txtMACStart_textChanged( const QString& )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_txtMACEnd_textEdited( const QString& )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_spxQuantity_valueChanged( int )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_txtCounterStart_textChanged( const QString& )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_cbxModel_currentTextChanged( const QString& arg1 )
{
    auto model = HxModel::Find( arg1 );
    if ( model == nullptr )
    {
        ui->txtDesign->setText( "-" );
        ui->txtProgram->setText( "-" );
    }
    else
    {
        ui->txtDesign->setText( model->design );
        ui->txtProgram->setText( model->ivProgram );
    }
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_txtName_textChanged( const QString& )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::ParamChanged( QStandardItem* )
{
    ShowBlocks();
    CheckInputs();
}

void HxLOTPropertyDialog::on_btnCreateOrChange_clicked()
{
    bool create = lot == nullptr;
    if ( create )
    {
        lot = HxLOT::Create();
    }
    PasteToLot( lot, create );
    HxLOT::SaveLot( lot );
    emit DataChanged();
    this->close();
    this->setResult( 1 );
}

void HxLOTPropertyDialog::on_chxRePrint_toggled( bool )
{
    ShowBlocks();
    CheckInputs();
}

