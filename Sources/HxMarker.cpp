#include "HxMarker.h"
#include "HxStopper.h"
#include <QTimer>
#include <QSerialPort>
#include <QTcpSocket>

#include "HxBarcodeSaver.h"
#include "HxLogSaver.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>



#include "HxMessage.h"


#include "HxLaserDevice.h"
#include "HxActuator.h"
#include "HxStopper.h"
#include "HxBarcodeReader.h"

#include "HxException.h"
#include "HxSystemError.h"


HxMarker::HxMarker( QObject* parent ) : QObject( parent )
{
    runFlag = false;
}

HxMarker::~HxMarker()
{

}

void HxMarker::Clear()
{
    lot.reset();
    model.reset();
    design.reset();
    stopper.reset();
}

bool HxMarker::Select( std::shared_ptr<HxLOT> lotinf )
{
    if ( lotinf == nullptr )
    {
        HxMessage::error( "Dữ liệu lot không hợp lệ !" );
        Clear();
        return false;
    }

    if ( lotinf->IsCompleted() )
    {
        HxMessage::error( "Lot này đã hoàn thành !" );
        Clear();
        return false;
    }

    auto _model = HxModel::Find( lotinf->modelName );
    if ( _model == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin model : " + lotinf->modelName );
        Clear();
        return false;
    }

    auto _design = HxDesign::Find( _model->design );
    if ( _design == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin thiết kế : " + _model->design );
        Clear();
        return false;
    }

    auto _stopper = HxStopper::Find( _model->stopper );
    if ( _stopper == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin stopper : " + QString::number( _model->stopper ) );
        Clear();
        return false;
    }

    // try setup
    try
    {
        HxLaserDevice::SetProgram( _design->name );
        HxActuator::SetCvWidth( _model->cvWidth );
        HxActuator::SetStopper( _model->stopper );

        lot = lotinf;
        model = _model;
        design = _design;
        stopper = _stopper;
        return true;
    }
    catch ( HxException ex )
    {
        HxSystemError::Instance()->ErrorReport( ex );
        //        Message::error(ex.message);
        Clear();
        return false;
    }
}

bool HxMarker::Mark( bool test )
{
    auto tempLot = std::make_shared<HxLOT>( HxLOT() );
    tempLot.get()[ 0 ] = lot.get()[ 0 ];
    int patternCnt = model->positions.size();

    if ( test )
    {
        for ( int i = 0; i < patternCnt; i++ )
        {
            HxPosition pos = model->positions[ i ];
            QMap<int, QString> blockDatas = HxBlock::gen( design, tempLot, model );


            HxLaserDevice::SetupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            HxLaserDevice::SetupBlockData( design->name, blockDatas );
            HxLaserDevice::Burn();
            tempLot->progress++;
        }
    }
    else
    {
        for ( int i = 0; i < patternCnt; i++ )
        {
            if ( tempLot->IsCompleted() ) continue;
            HxPosition pos = model->positions[ i ];
            QMap<int, QString> blockDatas = HxBlock::gen( design, tempLot, model );

            HxLaserDevice::SetupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            HxLaserDevice::SetupBlockData( design->name, blockDatas );
            HxLaserDevice::Burn();
            HxLogSaver::Save( tempLot, model, design );
            tempLot->progress++;
        }
        lot.get()[ 0 ] = tempLot.get()[ 0 ];
        HxLOT::SaveLot( lot );
    }
    return true;
}


bool HxMarker::IsBusy()
{
    return runFlag;
}

void HxMarker::Start()
{
    if ( runFlag )
    {
        return;
    }

    QMetaObject::invokeMethod( this, "task" );
}

void HxMarker::Stop()
{
    runFlag = false;
}

void HxMarker::Task()
{
    emit started();
    runFlag = true;
    QThread::msleep( 500 );
    while ( true )
    {
        if ( !runFlag )
            break;
        QThread::msleep( 50 );

        try
        {
            if ( HxBarcodeReader::IsHasData() )
            {
                HxBarcodeReader::Clear();
                QString code = HxBarcodeReader::Read().trimmed();
                if ( code.length() > 0 && code.startsWith( "ERROR" ) == false )
                {
                    HxBarcodeSaver::Save( code );
                    HxBarcodeReader::SendFeedback( true );
                }
                else
                {
                    HxBarcodeReader::SendFeedback( false );
                }
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Đọc barcode";
            HxSystemError::Instance()->ErrorReport( ex );

            HxException ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            HxSystemError::Instance()->ErrorReport( ex2 );
            runFlag = false;
            break;
        }

        try
        {
            if ( HxActuator::IsHasTrigger() )
            {
                HxActuator::ConfirmTrigger();
                bool status = Mark( false );
                HxActuator::SetMarkResult( status );
                if ( lot->IsCompleted() )
                {
                    HxActuator::SetCompleteBit();
                }
                emit printed( lot );
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Khắc";
            HxSystemError::Instance()->ErrorReport( ex );

            HxException ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            HxSystemError::Instance()->ErrorReport( ex2 );

            runFlag = false;
            break;
        }
    }
    runFlag = false;
    emit stopped();
}

/* ------------------------------------------------------ */
QThread HxMarker::worker;
HxMarker* HxMarker::_instance = nullptr;

void HxMarker::Initialize()
{
    worker.start();
}
void HxMarker::Terminate()
{
    worker.quit();
}

HxMarker* HxMarker::Instance()
{
    if ( _instance == nullptr )
    {
        _instance = new HxMarker();
        _instance->moveToThread( &worker );
    }
    return _instance;
}
