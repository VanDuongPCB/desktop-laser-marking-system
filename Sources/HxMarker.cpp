#include "HxMarker.h"
#include "HxStopper.h"
#include <QTimer>
#include <QSerialPort>
#include <QTcpSocket>

#include "HxLogger.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "QtConcurrent"



#include "HxMessage.h"


#include "HxLaser.h"
#include "HxPLC.h"
#include "HxStopper.h"
#include "HxBarcode.h"

#include "HxException.h"
#include "HxSystemError.h"

#include "HxDataGenerator.h"
#include "HxEvent.h"

#include "HxDefines.h"


HxMarker::HxMarker() : QObject( nullptr )
{
    m_settings.Load();
}

HxMarker::~HxMarker()
{

}

HxMarker::State HxMarker::GetState() const
{
    return m_state;
}

void HxMarker::Init()
{
    if ( m_state != eOnUnInit )
        return;

    QtConcurrent::run( &HxMarker::Task, this );
}

void HxMarker::DeInit()
{
    m_state = eOnStopping;
}

bool HxMarker::Setup( SetupMode mode, const QString& param )
{
    if ( m_state != eOnFree && m_state != eOnFinish )
    {
        HxMsgError( tr( "Đang khắc hoặc đang vận chuyển!" ), tr( "Cài đặt không thành công" ) );
        return false;
    }

    m_pLOT.reset();
    m_pModel.reset();
    m_pDesign.reset();
    m_pStopper.reset();

    if ( mode == eModeTransfer )
    {
        HxModelPtr pModel = ModelManager()->GetModel( param );
        if ( !pModel )
        {
            HxMsgError( tr( "Không tìm thấy dữ liệu model: %1!" ).arg( param ), tr( "Cài đặt không thành công" ) );
            return false;
        }

        try
        {
            PLC()->SetCvWidth( pModel->CvWidth() );
            PLC()->SetStopper( pModel->Stopper() );
        }
        catch ( HxException ex )
        {
            qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoError, ex.toJsonObject() ) );
            HxMsgError( ex.Message(), ex.Where() );
            return false;
        }

        m_pModel = pModel;
        qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerSetupChanged ) );
        return true;
    }

    if ( mode == eModeMarking )
    {
        HxLOTPtr pLOT = LOTManager()->GetLOT( param );
        if ( !pLOT )
        {
            HxMsgError( tr( "Không tìm thấy dữ liệu LOT: %1!" ).arg( param ), tr( "Cài đặt không thành công" ) );
            return false;
        }

        HxModelPtr pModel = ModelManager()->GetModel( pLOT->Model() );
        if ( !pModel )
        {
            HxMsgError( tr( "Không tìm thấy dữ liệu model: %1!" ).arg( pLOT->Model() ), tr( "Cài đặt không thành công" ) );
            return false;
        }

        HxDesignPtr pDesign = DesignManager()->GetDesign( pModel->Design() );
        if ( !pDesign )
        {
            HxMsgError( tr( "Không tìm thấy dữ liệu thiết kế: %1!" ).arg( pModel->Design() ), tr( "Cài đặt không thành công" ) );
            return false;
        }

        HxStopperPtr pStopper = StopperManager()->GetStopper( pModel->Stopper() );
        if ( !pStopper )
        {
            HxMsgError( tr( "Không tìm thấy dữ liệu stopper: %1!" ).arg( pModel->Stopper() ), tr( "Cài đặt không thành công" ) );
            return false;
        }

        m_pLOT = pLOT;
        m_pModel = pModel;
        m_pDesign = pDesign;
        m_pStopper = pStopper;

        try
        {
            Laser()->SetProgram( pDesign->Name() );
            PLC()->SetCvWidth( pModel->CvWidth() );
            PLC()->SetStopper( pModel->Stopper() );
        }
        catch ( HxException ex )
        {
            qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoError, ex.toJsonObject() ) );
            HxMsgError( ex.Message(), ex.Where() );
            return false;
        }

        qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerSetupChanged ) );
        if ( m_state == eOnFinish )
        {
            m_state = eOnFree;
            qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoFree ) );
        }
        return true;
    }

    return false;
}

void HxMarker::Transfer()
{
    m_state = eOnTransfering;
}

void HxMarker::Mark()
{
    m_state = eOnMarking;
}

void HxMarker::Test()
{
    m_state = eOnTesting;
}

void HxMarker::Pause()
{
    m_state = eOnFree;
    PLC()->SetEnable( false );
}

void HxMarker::Stop()
{
    m_state = eOnFree;
    PLC()->SetEnable( false );
}

HxLOTPtr HxMarker::LOT() const
{
    return m_pLOT;
}

HxModelPtr HxMarker::Model() const
{
    return m_pModel;
}

HxDesignPtr HxMarker::Design() const
{
    return m_pDesign;
}

void HxMarker::ReLoadSetting()
{
    m_settings.Load();
}

void HxMarker::CheckAndPostEvent( State& lastState, State currentState, HxEvent::Type eventType )
{
    if ( lastState == currentState )
        return;

    if ( currentState == eOnMarking )
    {
        PLC()->SetTransferMode( false );
        PLC()->SetEnable( true );
    }
    else if ( currentState == eOnTransfering )
    {
        PLC()->SetTransferMode( true );
        PLC()->SetEnable( true );
    }
    else if ( currentState == eOnFree )
    {
        PLC()->SetEnable( false );
    }
    else if ( currentState == eOnStopping )
    {
        PLC()->SetEnable( false );
    }

    qApp->postEvent( qApp, new HxEvent( eventType ) );
    lastState = currentState;
}

void HxMarker::Task()
{
    int iDebug = 0;
    m_state = eOnFree;
    State lastState = eOnUnInit;
    bool bNeedStop = false;
    while ( true )
    {
    START_LOOP:
        QThread::msleep( 20 );
        if ( bNeedStop )
            break;

        switch ( m_state )
        {
        case HxMarker::eOnFree:
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoFree );
            break;
        case HxMarker::eOnTransfering:
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoTransfer );
            break;
        case HxMarker::eOnFinish:
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoFinish );
            break;
        case HxMarker::eOnStopping:
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerStopped );
            bNeedStop = true;
            break;
        case HxMarker::eOnError:
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoError );
            break;



        case HxMarker::eOnTesting:
        {
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoTest );

            auto& positions = m_pModel->Positions();
            int patternCnt = positions.size();
            int blockCodeIndex = m_pDesign->IndexOfBlockCode();
            bool bIsSucceed = true;
            for ( auto& [index, position] : positions )
            {
                std::map<int, QString> dataBlocks = GenMarkData( m_pDesign, m_pLOT, m_pModel );

                try
                {
                    Laser()->SetupPosition( m_pDesign->Name(), position, m_pDesign, m_pStopper );
                    Laser()->SetupBlockData( m_pDesign->Name(), dataBlocks );
                    Laser()->Burn();
                    m_pLOT->SetProgress( m_pLOT->Progress() + 1 );
                    m_pLOT->Evaluate();
                }
                catch ( HxException ex )
                {
                    // ném exception
                    QMetaObject::invokeMethod( this, [ & ]()
                                               {
                                                   HxMsgError( tr( "Không khắc được.\n%1." ).arg( ex.Message() ),
                                                               tr( "Lỗi khắc thử." ) );
                                               }, Qt::BlockingQueuedConnection );
                    qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoError, ex.toJsonObject() ) );
                    m_state = eOnFree;
                    bIsSucceed = false;
                    break;
                }
            }
            if ( bIsSucceed )
            {
                QMetaObject::invokeMethod( this, [ & ]()
                                           {
                                               HxMsgInfo( tr( "Đã hoàn thành khắc bản test." ),
                                                           tr( "Hoàn thành khắc." ) );
                                           }, Qt::BlockingQueuedConnection );
            }
            m_state = eOnFree;
        }
        break;



        case HxMarker::eOnMarking:
        {
            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoMark );

            try
            {
                if ( Barcode()->IsHasData() )
                {
                    Barcode()->Clear();
                    QString code = Barcode()->Read().trimmed();
                    if ( code.length() > 0 && code.startsWith( "ERROR" ) == false )
                    {
                        ReturnCode saveCode{};
                        QMetaObject::invokeMethod( this, [ & ]()
                                                   {
                                                       saveCode = Logger()->SaveBarcode( code );
                                                   }, Qt::BlockingQueuedConnection );
                        Barcode()->SendFeedback( true );
                    }
                    else
                    {
                        Barcode()->SendFeedback( false );
                    }
                }
            }
            catch ( HxException ex )
            {
                QMetaObject::invokeMethod( this, [ & ]()
                                           {
                                               HxMsgError( tr( "Không đọc được barcode sau khi khắc." ),
                                                           tr( "Lỗi đọc barcode." ), false );
                                           }, Qt::BlockingQueuedConnection );

                qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoError, ex.toJsonObject() ) );
                m_state = eOnError;
                goto START_LOOP;
            }



            try
            {
                if ( PLC()->IsHasTrigger() )
                {
                    PLC()->ConfirmTrigger();

                    auto& positions = m_pModel->Positions();
                    int patternCnt = positions.size();
                    int blockCodeIndex = m_pDesign->IndexOfBlockCode();
                    bool bIsRePrint = ( m_pLOT->IsRePrint() ) || ( m_pModel->IsPrintLo() );

                    for ( auto& [index, position] : positions )
                    {
                        m_pLOT->Evaluate();
                        if ( m_pLOT->IsCompleted() )
                        {
                            m_state = eOnFinish;
                            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoFinish );
                            break;
                        }

                        std::map<int, QString> dataBlocks = GenMarkData( m_pDesign, m_pLOT, m_pModel );
                        QString code = dataBlocks[ blockCodeIndex ];

                        if ( !bIsRePrint )
                        {
                            ReturnCode checkCode{};
                            QMetaObject::invokeMethod( this, [ & ]()
                                                       {
                                                           checkCode = Logger()->CheckSerialExisting( code );
                                                           if ( checkCode != RtNormal )
                                                           {
                                                               if ( checkCode == RtDBDataExisting )
                                                               {
                                                                   HxMsgError( tr( "PHÁT HIỆN TRÙNG TEM\n"
                                                                                   "LIÊN LẠC QUẢN LÝ NGAY !!!\n\n"
                                                                                   "Quá trình khắc đã dừng lại do phát hiện lỗi!" ),
                                                                               tr( "Tên LOT: %1\n"
                                                                                   "Số seri trùng: %2" )
                                                                               .arg( m_pLOT->Name() )
                                                                               .arg( code ),
                                                                               tr( "Trùng lặp tem" ), true );
                                                               }
                                                               else
                                                               {
                                                                   HxMsgError( tr( "Không kiểm tra được trùng tem\n"
                                                                                   "Quá trình khắc đã dừng lại do phát hiện lỗi!" ),
                                                                               tr( "Lỗi kiểm tra trùng tem" ) );
                                                               }
                                                           }
                                                       }, Qt::BlockingQueuedConnection );

                            // chỗ này nên ném exeption
                            if ( checkCode != RtNormal )
                            {
                                PLC()->SetMarkResult( false );
                                m_state = eOnError;
                                goto START_LOOP;
                            }

                        }

                        Laser()->SetupPosition( m_pDesign->Name(), position, m_pDesign, m_pStopper );
                        Laser()->SetupBlockData( m_pDesign->Name(), dataBlocks );
                        Laser()->Burn();

                        qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerMarked ) );

                        m_pLOT->SetProgress( m_pLOT->Progress() + 1 );
                        m_pLOT->Evaluate();

                        ReturnCode codeSaveLOT{}, savePrintCode{};
                        QMetaObject::invokeMethod( this, [ & ]()
                                                   {
                                                       codeSaveLOT = LOTManager()->Save( m_pLOT );
                                                       savePrintCode = Logger()->SavePrint( m_pLOT, m_pModel, m_pDesign, dataBlocks );

                                                       if ( savePrintCode == RtDBDataExisting )
                                                       {
                                                           HxMsgError( tr( "PHÁT HIỆN TRÙNG TEM\n"
                                                                           "LIÊN LẠC QUẢN LÝ NGAY !!!\n\n"
                                                                           "Quá trình khắc đã dừng lại do phát hiện lỗi!" ),
                                                                       tr( "Tên LOT: %1\n"
                                                                           "Số seri trùng: %2" )
                                                                       .arg( m_pLOT->Name() )
                                                                       .arg( code ),
                                                                       tr( "Trùng lặp tem" ), true );
                                                       }
                                                   }, Qt::BlockingQueuedConnection );

                        // chỗ này nên ném exeption
                        if ( savePrintCode != RtNormal )
                        {
                            PLC()->SetMarkResult( false );
                            m_state = eOnError;
                            goto START_LOOP;
                        }

                        PLC()->SetMarkResult( true );

                        if ( m_pLOT->IsCompleted() )
                        {
                            PLC()->SetCompleteBit();
                            m_state = eOnFinish;
                            CheckAndPostEvent( lastState, m_state, HxEvent::eMarkerGoFinish );
                            break;
                        }

                        QThread::msleep( 50 );
                    }

                }
            }
            catch ( HxException ex )
            {
                QMetaObject::invokeMethod( this, [ & ]()
                                           {
                                               HxMsgError( tr( "Quá trình khắc đã dừng lại do phát hiện lỗi!" ),
                                                           tr( "Có lỗi trong quá trình khắc" ) );
                                           }, Qt::BlockingQueuedConnection );

                m_state = eOnError;
                qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerGoError, ex.toJsonObject() ) );
                m_state = eOnError;
                goto START_LOOP;
            }
        }
        break;
        default:
            break;
        }
    }
    m_state = eOnUnInit;
    qApp->postEvent( qApp, new HxEvent( HxEvent::eMarkerStopped ) );
}













//
//void HxMarker::Clear()
//{
//    lot.reset();
//    model.reset();
//    design.reset();
//    stopper.reset();
//}
//
//bool HxMarker::Select( std::shared_ptr<HxLOT> lotinf )
//{
//    //if ( lotinf == nullptr )
//    //{
//    //    HxMessage::error( "Dữ liệu lot không hợp lệ !" );
//    //    Clear();
//    //    return false;
//    //}
//
//    //if ( lotinf->IsCompleted() )
//    //{
//    //    HxMessage::error( "Lot này đã hoàn thành !" );
//    //    Clear();
//    //    return false;
//    //}
//
//    //auto _model = HxModel::Find( lotinf->modelName );
//    //if ( _model == nullptr )
//    //{
//    //    HxMessage::error( "Không tìm thấy thông tin model : " + lotinf->modelName );
//    //    Clear();
//    //    return false;
//    //}
//
//    //auto _design = HxDesign::Find( _model->design );
//    //if ( _design == nullptr )
//    //{
//    //    HxMessage::error( "Không tìm thấy thông tin thiết kế : " + _model->design );
//    //    Clear();
//    //    return false;
//    //}
//
//    //auto _stopper = HxStopper::Find( _model->stopper );
//    //if ( _stopper == nullptr )
//    //{
//    //    HxMessage::error( "Không tìm thấy thông tin stopper : " + QString::number( _model->stopper ) );
//    //    Clear();
//    //    return false;
//    //}
//
//    //// try setup
//    //try
//    //{
//    //    HxLaserDevice::SetProgram( _design->name );
//    //    PLC()->SetCvWidth( _model->cvWidth );
//    //    PLC()->SetStopper( _model->stopper );
//
//    //    lot = lotinf;
//    //    model = _model;
//    //    design = _design;
//    //    stopper = _stopper;
//    //    return true;
//    //}
//    //catch ( HxException ex )
//    //{
//    //    HxSystemError::Instance()->ErrorReport( ex );
//    //    //        Message::error(ex.message);
//    //    Clear();
//    //    return false;
//    //}
//    return false;
//}

//bool HxMarker::Mark( bool test )
//{
//    //auto tempLot = std::make_shared<HxLOT>( HxLOT() );
//    //tempLot.get()[ 0 ] = lot.get()[ 0 ];
//    //int patternCnt = model->positions.size();
//
//    //if ( test )
//    //{
//    //    for ( int i = 0; i < patternCnt; i++ )
//    //    {
//    //        HxPosition pos = model->positions[ i ];
//    //        std::map<int, QString> blockDatas = BlockDataGen ( design, tempLot, model );
//
//
//    //        HxLaserDevice::SetupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
//    //        HxLaserDevice::SetupBlockData( design->name, blockDatas );
//    //        HxLaserDevice::Burn();
//    //        tempLot->progress++;
//    //    }
//    //}
//    //else
//    //{
//    //    for ( int i = 0; i < patternCnt; i++ )
//    //    {
//    //        if ( tempLot->IsCompleted() ) continue;
//    //        HxPosition pos = model->positions[ i ];
//    //        std::map<int, QString> blockDatas = BlockDataGen( design, tempLot, model );
//
//    //        HxLaserDevice::SetupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
//    //        HxLaserDevice::SetupBlockData( design->name, blockDatas );
//    //        HxLaserDevice::Burn();
//    //        HxLogSaver::Save( tempLot, model, design );
//    //        tempLot->progress++;
//    //    }
//    //    lot.get()[ 0 ] = tempLot.get()[ 0 ];
//    //    HxLOT::SaveLot( lot );
//    //}
//    return true;
//}

//
//bool HxMarker::IsBusy()
//{
//    return runFlag;
//}
//
//void HxMarker::Start()
//{
//    if ( runFlag )
//    {
//        return;
//    }
//
//    QMetaObject::invokeMethod( this, "task" );
//}

//void HxMarker::Stop()
//{
//    runFlag = false;
//}
//
//void HxMarker::Task()
//{
//    emit started();
//    runFlag = true;
//    QThread::msleep( 500 );
//    while ( true )
//    {
//        if ( !runFlag )
//            break;
//        QThread::msleep( 50 );
//
//        try
//        {
//            if ( Barcode()->IsHasData() )
//            {
//                Barcode()->Clear();
//                QString code = Barcode()->Read().trimmed();
//                if ( code.length() > 0 && code.startsWith( "ERROR" ) == false )
//                {
//                    Barcode()->Save( code );
//                    Barcode()->SendFeedback( true );
//                }
//                else
//                {
//                    Barcode()->SendFeedback( false );
//                }
//            }
//        }
//        catch ( HxException ex )
//        {
//            ex.SetWhere( "Đọc barcode" );
//            HxSystemError::Instance()->ErrorReport( ex );
//
//            HxException ex2;
//            ex2.SetWhere( "Khắc" );
//            ex2.SetMessage( "Quá trình khắc đã dừng lại do phát hiện lỗi!" );
//            HxSystemError::Instance()->ErrorReport( ex2 );
//            runFlag = false;
//            break;
//        }
//
//        try
//        {
//            if ( PLC()->IsHasTrigger() )
//            {
//                PLC()->ConfirmTrigger();
//                bool status = Mark( false );
//                PLC()->SetMarkResult( status );
//                if ( lot->IsCompleted() )
//                {
//                    PLC()->SetCompleteBit();
//                }
//                emit printed( lot );
//            }
//        }
//        catch ( HxException ex )
//        {
//            ex.SetWhere( "Khắc" );
//            HxSystemError::Instance()->ErrorReport( ex );
//
//            HxException ex2;
//            ex2.SetWhere( "Khắc" );
//            ex2.SetMessage( "Quá trình khắc đã dừng lại do phát hiện lỗi!" );
//            HxSystemError::Instance()->ErrorReport( ex2 );
//
//            runFlag = false;
//            break;
//        }
//    }
//    runFlag = false;
//    emit stopped();
//}

HxMarker* Marker()
{
    static HxMarker instance;
    return &instance;
}