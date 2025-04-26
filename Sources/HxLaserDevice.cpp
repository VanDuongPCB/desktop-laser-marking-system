#include "HxLaserDevice.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "HxException.h"
#include "HxSystemError.h"
#include "HxSettings.h"
#include "HxMessage.h"

bool HxLaserDevice::detectPortExisting()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for ( auto& it : ports )
    {
        if ( it.portName() == HxSettings::laserPort )
        {
            return true;
        }
    }
    return false;
}

QString HxLaserDevice::detectError( QString data )
{
    return "";
}

QString HxLaserDevice::sendData( QString data, int timeout )
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for ( auto& it : ports )
    {
        if ( it.portName() == HxSettings::laserPort )
        {

            QSerialPort port( HxSettings::laserPort );
            port.setBaudRate( 9600 );

            if ( !port.open( QIODevice::ReadWrite ) )
            {
                throw HxException( "Không mở được cổng kết nối tới máy khắc laser !\nCổng : " + HxSettings::laserPort );
            }

            port.write( data.toStdString().c_str() );
            if ( !port.waitForBytesWritten( timeout ) )
            {
                port.close();
                throw HxException( "Gửi dữ liệu đến máy khắc laser thất bại !\nCổng : " + HxSettings::laserPort );
            }

            if ( !port.waitForReadyRead( timeout ) )
            {
                port.close();
                throw HxException( "Máy khắc laser không phản hồi !\nCổng : " + HxSettings::laserPort );
            }

            std::vector<char> buffer;
            char oneBuff[ 1 ];
            do
            {
                if ( port.bytesAvailable() )
                {
                    port.read( oneBuff, 1 );
                    if ( oneBuff[ 0 ] != 13 ) buffer.push_back( oneBuff[ 0 ] );
                    else break;
                }
                else if ( !port.waitForReadyRead( 20 ) )
                {
                    break;
                }
            } while ( true );
            buffer.push_back( 0 );
            port.close();
            QString feedback = QString::fromStdString( buffer.data() ).trimmed();
            return feedback;
        }
    }
    throw HxException( "Không có kết nối với máy laser trên cổng " + HxSettings::laserPort );
}

bool HxLaserDevice::setProgram( QString name )
{
    QString cmd = "GA," + name + "\r";
    QString fb = sendData( cmd, 3000 );
    if ( fb != "GA,0" )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }
    return true;
}

bool HxLaserDevice::setupBlockData( QString program, QMap<int, QString> data )
{
    QStringList items;
    items.push_back( "C2" );
    items.push_back( program );
    QList<int> blockIdxs = data.keys();
    std::sort( blockIdxs.begin(), blockIdxs.end() );

    for ( auto idx : blockIdxs )
    {
        if ( idx < 1 ) continue;
        items.push_back( QString::number( idx ).rightJustified( 3, '0' ) );
        items.push_back( data[ idx ] );
    }

    if ( items.length() <= 2 ) return true;
    QString cmd = items.join( "," ) + "\r";
    QString fb = sendData( cmd, 3000 );
    if ( fb != "C2,0" )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }
    return true;
}

bool HxLaserDevice::setupPosition( QString program, HxPosition pos, int stopper, HxDesign design )
{
    if ( pos.angle % 90 != 0 )
    {
        QStringList items = {
            "Phát hiện góc cài đặt tem không đúng (0,90,180,270)",
            "Góc hiện tại : " + QString::number( pos.angle )
        };
        throw HxException( items.join( '\n' ) );
    }

    // 1. lấy tọa độ hiện tại dx, dy
    QString cmdCUR = "B1," + program + ",000\r";
    QString fb = sendData( cmdCUR, 3000 );
    QStringList fbItems = fb.split( ',' );
    if ( fbItems.size() != 4 )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }


    double dx = fbItems[ 2 ].toDouble( 0 );
    double dy = fbItems[ 3 ].toDouble( 0 );

    // 2. di chuyển về gốc
    QStringList items;
    items.push_back( "AG" );
    items.push_back( program );
    items.push_back( QString::number( -dx, 'f', 3 ) );
    items.push_back( QString::number( -dy, 'f', 3 ) );
    items.push_back( "0000.00" );
    items.push_back( "0000" );
    QString cmdORG = items.join( "," ) + "\r";
    fb = sendData( cmdORG, 3000 );
    if ( fb != "AG,0" )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }



    // 3. xoay góc yêu cầu
    int angle = ( 90 * pos.angle / 90 ) % 360;
    if ( angle == 270 ) angle = -90;
    items.clear();
    items.push_back( "VG" );
    items.push_back( program );
    items.push_back( "0" );
    items.push_back( "0" );
    items.push_back( "0" );
    items.push_back( "0" );
    items.push_back( QString::number( angle ) );
    QString cmdRotate = items.join( "," ) + "\r";
    fb = sendData( cmdRotate, 3000 );
    if ( fb != "VG,0" )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }

    // 4. lấy thông tin stopper đang dùng
    auto stp = HxStopper::find( stopper );
    if ( stp == nullptr )
    {
        QStringList items = {
            "Không xác nhận được thông tin stopper !",
            "Stopper : " + QString::number( stopper )
        };
        throw HxException( items.join( '\n' ) );
    }

    // 5. tính toán lại vị trí theo từng góc
    double mx = 0;
    double my = 0;
    // 5.1. với góc 0
    if ( angle == 0 )
    {
        mx = stp->x + pos.x;
        my = stp->y + pos.y;
    }
    // 5.2. với góc 90 độ
    else if ( angle == 90 )
    {
        // thay đổi mx sẽ thay đổi y trên mẫu in
        mx = pos.y + stp->y;
        // thay đổi my sẽ thay đổi x trên mẫu in
        // my càng lớn thì x di chuyển theo chiều âm Ox;
        my = -( pos.x + stp->x + design.height );
    }

    // 5.3. với góc 180 độ
    else if ( angle == 180 )
    {
        // mx càng dương thì x trên bản in càng chạy về chiều âm Ox
        mx = -( pos.x + stp->x + design.width );
        my = -( pos.y + stp->y + design.height );
    }

    // 5.4 với góc 270 độ (-90 độ)
    else if ( angle == -90 )
    {
        mx = -( pos.y + stp->y + design.width );
        my = pos.x + stp->x;
    }
    else
    {

    }
    // 6. di chuyển đến vị trí đã tính
    if ( ( mx < -150 ) || ( mx > 150 ) || ( my < -150 ) || ( my > 150 ) )
    {
        QStringList items = {
            "Phát hiện tọa độ cần in vượt ngoài khoảng quy định (-150 ~ 150) !",
            "Tọa độ : " + QString::number( mx ) + "," + QString::number( my ),
            "Stopper : " + QString::number( stopper ) + ", offset = " + QString::number( stp->x ) + "," + QString::number( stp->y )
        };
        throw HxException( items.join( '\n' ) );
    }

    items.clear();
    items.push_back( "AG" );
    items.push_back( program );
    items.push_back( QString::number( mx, 'f', 3 ) );
    items.push_back( QString::number( my, 'f', 3 ) );
    items.push_back( "0000.00" );
    items.push_back( "0000" );
    QString cmdMove = items.join( "," ) + "\r";
    fb = sendData( cmdMove, 3000 );
    if ( fb != "AG,0" )
    {
        QStringList items = {
            "Dữ liệu phản hồi từ máy khắc laser không đúng !",
            "Dữ liệu : " + fb
        };
        throw HxException( items.join( '\n' ) );
    }
    return true;
}

bool HxLaserDevice::burn()
{
    QString fb = sendData( "NT\r" );
    if ( fb == "NT,0" ) return true;
    else
    {
        QString code = fb.split( ',' ).last();
        throw HxException( "Lệnh in thất bại.\nMã lỗi : " + code );
    }
    return true;
}
