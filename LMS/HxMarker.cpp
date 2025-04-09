#include "HxMarker.h"
#include "HxStopper.h"
#include <QTimer>
#include <QSerialPort>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QFile>
#include <QDir>


#include "HxMessage.h"


#include "HxLaser.h"
#include "HxPLC.h"
#include "HxStopper.h"
#include "HxBarcode.h"

#include "HxException.h"
#include "HxSystemError.h"
#include "HxFileManager.h"

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
        HxShowError( "Dữ liệu lot không hợp lệ !" );
        clear();
        return false;
    }

    if ( lotinf->isCompleted() )
    {
        HxShowError( "Lot này đã hoàn thành !" );
        clear();
        return false;
    }

    auto _model = HxModel::find( lotinf->modelName );
    if ( _model == nullptr )
    {
        HxShowError( "Không tìm thấy thông tin model : " + lotinf->modelName );
        clear();
        return false;
    }

    auto _design = HxDesign::find( _model->design );
    if ( _design == nullptr )
    {
        HxShowError( "Không tìm thấy thông tin thiết kế : " + _model->design );
        clear();
        return false;
    }

    auto _stopper = HxStopper::find( _model->stopper );
    if ( _stopper == nullptr )
    {
        HxShowError( "Không tìm thấy thông tin stopper : " + QString::number( _model->stopper ) );
        clear();
        return false;
    }

    // try setup
    try
    {
        GetLaserMachine()->setProgram( _design->name );
        GetPLC()->setCvWidth( _model->cvWidth );
        GetPLC()->setStopper( _model->stopper );

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
            QMap<int, QString> blockDatas = HxBlockInfo::gen( design, tempLot, model );


            GetLaserMachine()->setupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            GetLaserMachine()->setupBlockData( design->name, blockDatas );
            GetLaserMachine()->burn();
            tempLot->progress++;
        }
    }
    else
    {
        for ( int i = 0; i < patternCnt; i++ )
        {
            if ( tempLot->isCompleted() ) continue;
            HxPosition pos = model->positions[ i ];
            QMap<int, QString> blockDatas = HxBlockInfo::gen( design, tempLot, model );

            GetLaserMachine()->setupPosition( design->name, pos, model->stopper, design.get()[ 0 ] );
            GetLaserMachine()->setupBlockData( design->name, blockDatas );
            GetLaserMachine()->burn();
            save( tempLot, model, design );
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
    runFlag = true;
    QThread::msleep( 500 );
    emit started();
    while ( true )
    {
        if ( !runFlag ) break;
        QThread::msleep( 50 );

        try
        {
            if ( GetBarcode()->hasData() )
            {
                GetBarcode()->clear();
                QString code = GetBarcode()->read().trimmed();
                if ( code.length() > 0 && code.startsWith( "ERROR" ) == false )
                {
                    GetBarcode()->save( code );
                    GetBarcode()->sendFeedback( true );
                }
                else
                {
                    GetBarcode()->sendFeedback( false );
                }
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Đọc barcode";
            HxSystemError::instance()->errorReport( ex );
        }

        try
        {
            if ( GetPLC()->hasTrigger() )
            {
                GetPLC()->confirmTrigger();
                bool status = mark( false );
                GetPLC()->setMarkResult( status );
                if ( lot->isCompleted() )
                {
                    GetPLC()->setCompleteBit();
                }
                emit printed( lot );
            }
        }
        catch ( HxException ex )
        {
            ex.where = "Khắc";
            HxSystemError::instance()->errorReport( ex );
        }
    }
    runFlag = false;
    emit stopped();
}

void HxMarker::save( std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model, std::shared_ptr<HxDesign> design )
{
    if ( lot == nullptr )
    {
        return;
    }

    if ( model == nullptr )
    {
        return;
    }

    if ( design == nullptr )
    {
        return;
    }

    QString dir = GetFileManager()->GetPath(lot->isRePrint ? HxFileManager::eDBRePrintLogDir : HxFileManager::eDBPrintLogDir);
    QDir().mkdir( dir );

    QString fileName = QDateTime::currentDateTime().toString( "yyyyMMdd" ) + ".csv";
    QString filePath = dir + "/" + fileName;

    QFile fileWriter( filePath );
    if ( !fileWriter.exists() )
    {
        if ( fileWriter.open( QIODevice::WriteOnly ) )
        {
            fileWriter.write( "TIME,LOT,MODEL,DESIGN,DATA-1,DATA-2,DATA-3,DATA-4,DATA-5,DATA-6,DATA-7,DATA-8,DATA-9,DATA-10" );
            fileWriter.close();
        }
    }

    QStringList itemData;
    itemData.push_back( "\n" + QDateTime::currentDateTime().toString( "HH:mm:ss" ) );
    itemData.push_back( lot->name );
    itemData.push_back( model->name );
    itemData.push_back( design->name );



    for ( int i = 1; i <= 10; i++ )
    {
        if ( design->blocks.contains( i ) )
        {
            itemData.push_back( HxBlockInfo::gen( design->blocks[ i ].data, lot, model ) );
        }
        else
        {
            itemData.push_back( "" );
        }
    }

    QString data = itemData.join( "," );
    if ( fileWriter.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
    {
        fileWriter.write( data.toStdString().c_str(), data.length() );
        fileWriter.close();
    }
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
