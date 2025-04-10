#include "HxLOTPropertyDialog.h"
#include "ui_HxLOTPropertyDialog.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"
#include "HxConvert.h"
#include "HxMessage.h"

HxLOTPropertyDialog::HxLOTPropertyDialog( const QString& lotName, HxLOTPtrMap LOTs, QWidget* parent ) : QDialog( parent ), ui( new Ui::LotPropertiesDialog )
{
    ui->setupUi( this );
    m_LOTs = LOTs;
    ui->tbvParams->setHeaders( { "Thông tin","Giá trị" } );
    ui->tbvBlocks->setHeaders( { "Block","Nội dung","Độ dài","Định dạng" } );
    ui->tbvBlocks->setColumnWidth( 1, 300 );

    OnInit(lotName);
    showInfo();
    ShowPrintLo();
    showParams();
    showBlocks();

    connect(ui->cbxModel, &QComboBox::currentTextChanged, this, &HxLOTPropertyDialog::OnModelChanged);
    connect(ui->btnCreateOrChange, &QPushButton::clicked, this, &HxLOTPropertyDialog::OnApply);
}

HxLOTPropertyDialog::~HxLOTPropertyDialog()
{
    delete ui;
}

void HxLOTPropertyDialog::OnInit(const QString& lotName)
{
    QSignalBlocker blocker(this);
    m_pLOT = GetLOTManager()->GetLOT(lotName);
    m_bIsCreate = m_pLOT == nullptr;
    if(m_bIsCreate)
    {
        m_pLOT = GetLOTManager()->Create();
        setWindowTitle("Tạo LOT mới");
        ui->btnCreateOrChange->setText("Tạo LOT mới");
        ui->txtName->setReadOnly(false);
    }
    else
    {
        setWindowTitle("Thay đổi thông tin LOT");
        ui->btnCreateOrChange->setText("Cập nhật LOT");
        ui->txtName->setReadOnly(true);
    }

    ui->cbxModel->addItem("");
    ui->cbxModel->addItems(GetModelManager()->Names());

    switch (m_pLOT->Status()) {
    case HxLOT::ePending:
        ui->cbxModel->setEnabled( true );
        ui->txtMACStart->setEnabled( true );
        ui->txtCounter->setEnabled( true );
        ui->txtMACEnd->setEnabled( true );
        ui->spxQuantity->setEnabled( true );
        ui->txtCounterStart->setEnabled( true );
        break;
    case HxLOT::eProduct:
        ui->cbxModel->setEnabled( false );
        ui->txtMACStart->setEnabled( false );
        ui->txtCounter->setEnabled( true );
        ui->txtMACEnd->setEnabled( true );
        ui->spxQuantity->setEnabled( true );
        ui->txtCounterStart->setEnabled( false );
        break;

    case HxLOT::eCompleted:
        ui->cbxModel->setEnabled( false );
        ui->txtMACStart->setEnabled( false );
        ui->txtCounter->setEnabled( true );
        ui->txtMACEnd->setEnabled( true );
        ui->spxQuantity->setEnabled( true );
        ui->txtCounterStart->setEnabled( false );
        break;
    default:
        break;
    }

    m_pModel = GetModelManager()->GetModel(m_pLOT->m_modelName);
}

void HxLOTPropertyDialog::showInfo()
{
    QSignalBlocker blocker(this);
    ui->txtName->setText( m_pLOT->m_name );
    ui->cbxModel->setCurrentText( m_pLOT->m_modelName );
    ui->txtCounter->setText( m_pLOT->Counter() );
    ui->txtMACEnd->setText( m_pLOT->m_macEnd );
    ui->spxQuantity->setValue( m_pLOT->m_quantity );
    ui->txtCounterStart->setText( m_pLOT->m_counterStart );
}

void HxLOTPropertyDialog::ShowPrintLo()
{
    ui->chxRePrint->setVisible(false);
    auto pModel = GetModelManager()->GetModel(m_pLOT->m_modelName);
    if(pModel == nullptr || pModel->IsPrintLo() == false)
        return;
    ui->chxRePrint->setVisible(true);
}

void HxLOTPropertyDialog::showParams()
{
    QSignalBlocker blocker(this);
    disconnect( ui->tbvParams->dataTable(), &QStandardItemModel::itemChanged, this, &HxLOTPropertyDialog::OnParamChanged );

    QStringList paramNames = {};
    {
        QStringList keys = m_pLOT->m_comments.keys();
        for ( auto& key : keys )
        {
            if ( !paramNames.contains( key ) ) paramNames.push_back( key );
        }
    }

    for ( auto& [name, pLOT] : m_LOTs )
    {
        QStringList keys = pLOT->m_comments.keys();
        for ( auto& key : keys )
        {
            if ( !paramNames.contains( key ) ) paramNames.push_back( key );
        }
    }

    int rows = paramNames.size();
    ui->tbvParams->setRowCount( rows );

    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvParams->setText( row, 0, paramNames[ row ] );
        ui->tbvParams->setText( row, 1, "" );
        if ( m_pLOT->m_comments.contains( paramNames[ row ] ) )
        {
            ui->tbvParams->setText( row, 1, m_pLOT->m_comments.value( paramNames[ row ] ) );
        }
    }
    connect( ui->tbvParams->dataTable(), &QStandardItemModel::itemChanged, this, &HxLOTPropertyDialog::OnParamChanged );
}

void HxLOTPropertyDialog::showBlocks()
{
    QSignalBlocker blocker(this);
    ui->tbvBlocks->setRowCount( 0 );
    auto pModel = GetModelManager()->GetModel( m_pLOT->m_modelName );
    if(!pModel)
        return;

    auto pDesign = HxDesign::find( pModel->Design() );
    if(!pDesign)
        return;

    QList<int> nums = pDesign->blocks.keys();

    ui->tbvBlocks->setRowCount( nums.size() );
    for ( int i = 0; i < nums.size(); i++ )
    {
        int blockIdx = nums[ i ];
        ui->tbvBlocks->setText( i, "Block", QString::number( nums[ i ] ).rightJustified( 3, '0' ) );

        QString data = HxBlockInfo::gen( pDesign->blocks[ blockIdx ].data, m_pLOT, pModel );
        ui->tbvBlocks->setText( i, "Nội dung", data );

        ui->tbvBlocks->setText( i, "Định dạng", pDesign->blocks[ blockIdx ].data );

        int len = data.length();
        int setupLen = pDesign->blocks[ blockIdx ].textLen;
        bool match = len == setupLen;
        ui->tbvBlocks->setText( i, "Độ dài", QString::number( len ) + "/" + QString::number( setupLen ) );

        QColor color = QColor( 0, 128, 0 );
        if ( !match ) color = QColor( 128, 0, 0 );
        for ( int col = 0; col < ui->tbvBlocks->dataTable()->columnCount(); col++ )
        {
            ui->tbvBlocks->item( i, col )->setForeground( color );
        }
    }
}

bool HxLOTPropertyDialog::checkLotName()
{
    if(!m_bIsCreate)
        return true;

    QString lotName = ui->txtName->text().trimmed().toUpper();

    if(lotName.isEmpty())
    {
        HxMsgError(tr("Tên LOT không được để trống!"),
                   tr("Lỗi"));
        return false;
    }

    for ( auto& [name, pLOT] : m_LOTs )
    {
        if(pLOT->m_name == lotName)
        {
            HxMsgError(tr("Phát hiện trùng tên LOT!\n"
                          "Trường hợp bắt buộc tên LOT phải giống, hãy thêm vào phía sau một cụm từ bắt đầu bằng \"-RE\".\n"
                          "Ví dụ:\n"
                          "TÊN LOT-RE01"),
                       tr("Trùng lặp tên LOT"));
            return false;
        }
    }
    return true;
}

bool HxLOTPropertyDialog::checkSeriRange()
{
    if ( ui->chxRePrint->isVisible() )
        return true;

    auto pModel = GetModelManager()->GetModel(m_pLOT->Model());
    if(!pModel)
        return true;

    auto pDesign = HxDesign::find(pModel->Design());
    if(!pDesign)
        return true;

    int indexBlockCode = pDesign->indexOfBlockCode();
    if ( indexBlockCode < 1 )
        return true;
    HxBlockInfo block = pDesign->blocks[ indexBlockCode ];

    HxLOTPtr pFrom = m_pLOT->Clone();
    pFrom->SetProgress(0);
    QString startSerial = HxBlockInfo::gen( block.data, pFrom, pModel );
    pFrom->SetProgress(pFrom->Quantity()-1);
    QString endSerial = HxBlockInfo::gen( block.data, pFrom, pModel );


    for(auto &[name, pLOT] : m_LOTs)
    {
        if(pFrom->Name() == pLOT->Name())
            continue;

        if(pFrom->Model() != pLOT->Model())
            continue;

        if(pFrom->CounterStart().length() != pLOT->CounterStart().length())
            continue;

        HxLOTPtr pTo = pLOT->Clone();
        pTo->SetProgress( 0);
        QString startSerial2 = HxBlockInfo::gen( block.data, pTo, pModel );
        pTo->SetProgress(pTo->Quantity() - 1);
        QString endSerial2 = HxBlockInfo::gen( block.data, pTo, pModel );

        std::vector<QString> serials {startSerial,endSerial,startSerial2,endSerial2};
        std::sort(serials.begin(), serials.end());
        bool bIsInter = (serials[1] == serials[2]) || (startSerial == serials[0] && endSerial != serials[1]) || (startSerial == serials[2] && endSerial != serials[3]);
        if(bIsInter)
        {
            HxMsgError(tr("Phát hiện trùng lặp dải seri với lot: %1!\n"
                          "Hãy liên lạc với quản đốc!").arg(pLOT->m_name),
                       tr("Tên LOT: %1\n"
                          "Dải serial: %2 - %3").arg(pLOT->m_name).arg(startSerial2).arg(endSerial2),
                       tr("Trùng lặp dải MAC"),
                       true);
            return false;
        }
    }





    // if ( ui->chxRePrint->isChecked() ) return true;
    // QString modelName = ui->cbxModel->currentText().trimmed().toUpper();
    // auto model = GetModelManager()->GetModel( modelName );
    // if ( model == nullptr ) return false;

    // auto design = HxDesign::find( model->Design() );
    // if ( design == nullptr ) return false;
    // int indexBlockCode = design->indexOfBlockCode();
    // if ( indexBlockCode < 1 ) return false;
    // HxBlockInfo block = design->blocks[ indexBlockCode ];

    // auto fromLot = std::make_shared<HxLOT>( HxLOT() );
    // pasteToLot( fromLot, true );
    // fromLot->progress = 0;
    // QString startSerial = HxBlockInfo::gen( block.data, fromLot, model );
    // fromLot->progress = fromLot->quantity - 1;
    // QString endSerial = HxBlockInfo::gen( block.data, fromLot, model );

    // for ( auto& it : HxLOT::items )
    // {
    //     if ( it == lot ) continue;
    //     if ( it->modelName != model->Name() ) continue;
    //     if ( it->name == fromLot->name ) continue;
    //     auto toLot = it;

    //     //        // con mẹ nó, chỗ này không backup progress nên mới xảy ra progess = quantity - 1
    //     //        // khi tạo hoặc sửa lot
    //     //        toLot->progress = 0;
    //     //        QString startSerial2 = BlockInfo::gen(block.data, toLot,model);
    //     //        toLot->progress = toLot->quantity - 1;
    //     //        QString endSerial2 = BlockInfo::gen(block.data, toLot, model);

    //             // phải sửa lại như thế này
    //     int backupProgress = toLot->progress;
    //     toLot->progress = 0;
    //     QString startSerial2 = HxBlockInfo::gen( block.data, toLot, model );
    //     toLot->progress = toLot->quantity - 1;
    //     QString endSerial2 = HxBlockInfo::gen( block.data, toLot, model );
    //     // backup
    //     toLot->progress = backupProgress;

    //     QStringList checkItems = { startSerial, endSerial };
    //     if ( checkItems.contains( startSerial2 ) ) return false;
    //     if ( checkItems.contains( endSerial2 ) ) return false;
    //     checkItems.push_back( startSerial2 );
    //     checkItems.push_back( endSerial2 );
    //     checkItems.sort();

    //     if ( startSerial == checkItems[ 0 ] && endSerial == checkItems[ 1 ] ) continue;
    //     if ( startSerial == checkItems[ 2 ] && endSerial == checkItems[ 3 ] ) continue;
    //     return false;
    // }
    return true;
}

bool HxLOTPropertyDialog::checkMacs()
{
    if ( ui->chxRePrint->isVisible() )
        return true;

    QString lotName = ui->txtName->text().trimmed().toUpper();
    QString macStart = ui->txtMACStart->text().trimmed();
    QString macEnd = ui->txtMACEnd->text().trimmed();
    if(macStart.isEmpty() && macEnd.isEmpty())
        return true;

    if(macStart.length() != 12)
    {
        HxMsgError(tr("MAC bắt đầu không hợp lệ!"),tr("MAC không hợp lệ"));
        return false;
    }

    if(macEnd.length() != 12)
    {
        HxMsgError(tr("MAC kết thúc không hợp lệ!"),tr("MAC không hợp lệ"));
        return false;
    }

    std::string smac = ( macStart + macEnd ).toStdString();
    for ( int i = 0; i < smac.length(); i++ )
    {
        char c = smac[ i ];
        if ( ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) ) continue;
        HxMsgError(tr("MAC chứa ký tự không hợp lệ!"),tr("MAC không hợp lệ"));
        return false;
    }

    uint64_t numMACStart = Uint64FromHexString(macStart);
    uint64_t numMACEnd = Uint64FromHexString(macEnd);
    if(numMACStart > numMACEnd)
    {
        HxMsgError(tr("MAC bắt đầu không được lớn hơn MAC kết thúc!"),tr("MAC không hợp lệ"));
        return false;
    }

    for(auto &[name, pLOT] : m_LOTs)
    {
        if(pLOT->m_name == lotName)
            continue;

        if(pLOT->m_macStart.length() + pLOT->m_macEnd.length() != 24)
            continue;

        uint64_t checkMACStart = Uint64FromHexString(pLOT->m_macStart);
        uint64_t checkMACEnd = Uint64FromHexString(pLOT->m_macEnd);
        if(checkMACStart > checkMACEnd)
            continue;

        std::vector<uint64_t> macs = {numMACStart, numMACEnd, checkMACStart, checkMACEnd};
        std::sort(macs.begin(), macs.end());

        bool bIsInter = (macs[1] == macs[2]) || (numMACStart == macs[0] && numMACEnd != macs[1]) || (numMACStart == macs[2] && numMACEnd != macs[3]);

        if(bIsInter)
        {
            HxMsgError(tr("Phát hiện trùng lặp dải MAC với lot: %1!\n"
                          "Hãy liên lạc với quản đốc!").arg(pLOT->m_name),
                       tr("Tên LOT: %1\n"
                          "Dải MAC: %2 - %3").arg(pLOT->m_name).arg(pLOT->m_macStart).arg(pLOT->m_macEnd),
                       tr("Trùng lặp dải MAC"),
                       true);
            return false;
        }
    }
    return true;
}

bool HxLOTPropertyDialog::checkModelInfo()
{
    QString modelName = ui->cbxModel->currentText();
    auto pModel = GetModelManager()->GetModel( modelName );
    if(!pModel)
    {
        HxMsgError(tr("Không tồn tại model: %1!").arg(modelName),
                   tr("Thông tin cài đặt không đúng"));
        return false;
    }

    auto pDesign = HxDesign::find( pModel->Design() );
    if(!pDesign)
    {
        HxMsgError(tr("Không tồn tại mẫu tem: %1!").arg(pModel->Design()),
                   tr("Tên mẫu tem: %1").arg(pModel->Design()),
                   tr("Thông tin cài đặt không đúng"));
        return false;
    }

    return true;
}

bool HxLOTPropertyDialog::checkBlocks()
{
    int rows = ui->tbvBlocks->dataTable()->rowCount();
    for ( int row = 0; row < rows; row++ )
    {
        QColor color = ui->tbvBlocks->item( row, 0 )->foreground().color();
        int r = color.red();
        int g = color.green();
        if ( r > g )
        {
            HxMsgError(tr("Phát hiện một số block có dữ liệu không đúng!"),
                       tr("Thông tin block không đúng"));
            return false;
        }
    }
    return true;
}

bool HxLOTPropertyDialog::checkInputs()
{
    return checkLotName() && checkModelInfo() && checkSeriRange() && checkMacs() && checkBlocks();
}

void HxLOTPropertyDialog::OnParamChanged( QStandardItem* item)
{
    int row = item->row();
    QString key = ui->tbvParams->item(row,0)->text();
    QString value = ui->tbvParams->item(row,1)->text().trimmed().toUpper();
    m_pLOT->SetValue(key,value);
    showBlocks();
    checkInputs();
}

void HxLOTPropertyDialog::OnModelChanged()
{
    m_pLOT->m_modelName = ui->cbxModel->currentText();
    auto pModel = GetModelManager()->GetModel( m_pLOT->m_modelName );
    if(pModel)
    {
        ui->txtDesign->setText( pModel->Design() );
        ui->txtProgram->setText( pModel->IVProgram() );
        ui->chxRePrint->setVisible(pModel->IsPrintLo());
    }
    else
    {
        ui->txtDesign->setText( "-" );
        ui->txtProgram->setText( "-" );
        ui->chxRePrint->setVisible(false);
    }
    showBlocks();
}

void HxLOTPropertyDialog::OnApply()
{
    if(!checkInputs())
        return;
    GetLOTManager()->Save(m_pLOT);
    close();
    setResult(1);
}
