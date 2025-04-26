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

void HxMarker::clear()
{
    lot.reset();
    model.reset();
    design.reset();
    stopper.reset();
}

bool HxMarker::select( std::shared_ptr<HxLOT> lotinf )
{
    if ( lotinf == nullptr )
    {
        HxMessage::error( "Dữ liệu lot không hợp lệ !" );
        clear();
        return false;
    }

    if ( lotinf->isCompleted() )
    {
        HxMessage::error( "Lot này đã hoàn thành !" );
        clear();
        return false;
    }

    auto _model = HxModel::find( lotinf->modelName );
    if ( _model == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin model : " + lotinf->modelName );
        clear();
        return false;
    }

    auto _design = HxDesign::find( _model->design );
    if ( _design == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin thiết kế : " + _model->design );
        clear();
        return false;
    }

    auto _stopper = HxStopper::find( _model->stopper );
    if ( _stopper == nullptr )
    {
        HxMessage::error( "Không tìm thấy thông tin stopper : " + QString::number( _model->stopper ) );
        clear();
        return false;
    }

    // try setup
    try
    {
        HxLaserDevice::setProgram( _design->name );
        HxActuator::setCvWidth( _model->cvWidth );
        HxActuator::setStopper( _model->stopper );

        lot = lotinf;
        model = _model;
        design = _design;
        stopper = _stopper;
        return true;
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
        //        Message::error(ex.message);
        clear();
        return false;
    }
}

bool HxMarker::mark( bool test )
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


            HxLaserDevice::setupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            HxLaserDevice::setupBlockData( design->name, blockDatas );
            HxLaserDevice::burn();
            tempLot->progress++;
        }
    }
    else
    {
        for ( int i = 0; i < patternCnt; i++ )
        {
            if ( tempLot->isCompleted() ) continue;
            HxPosition pos = model->positions[ i ];
            QMap<int, QString> blockDatas = HxBlock::gen( design, tempLot, model );

            HxLaserDevice::setupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            HxLaserDevice::setupBlockData( design->name, blockDatas );
            HxLaserDevice::burn();
            HxLogSaver::save( tempLot, model, design );
            tempLot->progress++;
        }
        lot.get()[ 0 ] = tempLot.get()[ 0 ];
        HxLOT::saveLot( lot );
    }
    return true;
}


bool HxMarker::isBusy()
{
    return runFlag;
}

void HxMarker::start()
{
    if ( runFlag )
    {
        return;
    }

    QMetaObject::invokeMethod( this, "task" );
}

void HxMarker::stop()
{
    runFlag = false;
}

void HxMarker::task()
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
            if ( HxBarcodeReader::hasData() )
            {
                HxBarcodeReader::clear();
                QString code = HxBarcodeReader::read().trimmed();
                if ( code.length() > 0 && code.startsWith( "ERROR" ) == false )
                {
                    HxBarcodeSaver::save( code );
                    HxBarcodeReader::sendFeedback( true );
                }
                else
                {
                    HxBarcodeReader::sendFeedback( false );
                }
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Đọc barcode";
            HxSystemError::instance()->errorReport( ex );

            HxException ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            HxSystemError::instance()->errorReport( ex2 );
            runFlag = false;
            break;
        }

        try
        {
            if ( HxActuator::hasTrigger() )
            {
                HxActuator::confirmTrigger();
                bool status = mark( false );
                HxActuator::setMarkResult( status );
                if ( lot->isCompleted() )
                {
                    HxActuator::setCompleteBit();
                }
                emit printed( lot );
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Khắc";
            HxSystemError::instance()->errorReport( ex );

            HxException ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            HxSystemError::instance()->errorReport( ex2 );

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

void HxMarker::initialize()
{
    worker.start();
}
void HxMarker::terminate()
{
    worker.quit();
}

HxMarker* HxMarker::instance()
{
    if ( _instance == nullptr )
    {
        _instance = new HxMarker();
        _instance->moveToThread( &worker );
    }
    return _instance;
}
