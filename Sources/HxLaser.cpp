#include "HxLaser.h"

#include "QApplication"
#include "QSerialPort"
#include "QSerialPortInfo"

#include "HxException.h"
#include "HxSystemReport.h"
#include "HxSettings.h"
#include "HxMessage.h"
#include "HxEvent.h"


QString HxLaser::SendData( const QString& data, int timeout )
{
    QString portName = m_settings.String( LaserConnPort );
    QSerialPort port( portName );
    port.setBaudRate( 9600 );

    if ( !port.open( QIODevice::ReadWrite ) )
    {
        throw HxException( tr( "Không mở được cổng kết nối tới máy khắc laser!\nCổng: %1" ).arg( portName ) );
    }

    port.write( data.toStdString().c_str() );
    if ( !port.waitForBytesWritten( timeout ) )
    {
        port.close();
        throw HxException( tr( "Gửi dữ liệu đến máy khắc laser thất bại!\nCổng: %1" ).arg( portName ) );
    }

    if ( !port.waitForReadyRead( timeout ) )
    {
        port.close();
        throw HxException( tr( "Máy khắc laser không phản hồi!\nCổng: %1" ).arg( portName ) );
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

HxLaser::HxLaser() : QObject( nullptr )
{
    m_settings.Load();
    qApp->installEventFilter( this );
}

bool HxLaser::SetProgram( const QString& name )
{
    QString cmd = "GA," + name + "\r";
    try
    {
        QString fb = SendData( cmd, 3000 );
        if ( fb != "GA,0" )
        {
            QString msg = tr( "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
                              "Dữ liệu: %1" )
                .arg( fb );
            throw HxException( "Laser", msg );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
    }
    
    return true;
}

bool HxLaser::SetupBlockData( const QString& program, std::map<int, QString> dataMap )
{
    QStringList items;
    items.push_back( "C2" );
    items.push_back( program );

    for ( auto& [index, data] : dataMap )
    {
        if ( index < 1 )
            continue;
        items.push_back( QString::number( index ).rightJustified( 3, '0' ) );
        items.push_back( data );
    }

    if ( items.length() <= 2 ) return true;
    QString cmd = items.join( "," ) + "\r";
    try
    {
        QString fb = SendData( cmd, 3000 );
        if ( fb != "C2,0" )
        {
            QString cmd = tr(
                "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
                "Dữ liệu: %1" )
                .arg( fb );
            throw HxException( cmd );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
    }
    
    return true;
}

bool HxLaser::SetupPosition( const QString& program, HxPosition pos, int stopper, HxDesignPtr pDesign )
{
    if ( pos.angle % 90 != 0 )
    {
        QString cmd = tr(
            "Phát hiện góc cài đặt tem không đúng (0,90,180,270)\n"
            "Góc hiện tại: %1" )
            .arg( pos.angle );
        throw HxException( "Laser", cmd);
    }

    // 1. lấy tọa độ hiện tại dx, dy
    QString cmdCUR = "B1," + program + ",000\r";
    QString fb = SendData( cmdCUR, 3000 );
    QStringList fbItems = fb.split( ',' );
    if ( fbItems.size() != 4 )
    {
        QString cmd = tr(
            "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
            "Dữ liệu: %1" )
            .arg( fb );
        throw HxException( cmd );
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
    try
    {
        fb = SendData( cmdORG, 3000 );
        if ( fb != "AG,0" )
        {
            QString cmd = tr(
                "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
                "Dữ liệu: %1" )
                .arg( fb );
            throw HxException( cmd );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
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
    try
    {
        fb = SendData( cmdRotate, 3000 );
        if ( fb != "VG,0" )
        {
            QString cmd = tr(
                "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
                "Dữ liệu: %1" )
                .arg( fb );
            throw HxException( cmd );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
    }
    
    // 4. lấy thông tin stopper đang dùng
    auto stp = StopperManager()->GetStopper( stopper );
    if ( stp == nullptr )
    {
        QString cmd = tr(
            "Không xác nhận được thông tin stopper!\n"
            "Stopper: %1" )
            .arg( stopper );
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
        my = -( pos.x + stp->x + pDesign->Height() );
    }

    // 5.3. với góc 180 độ
    else if ( angle == 180 )
    {
        // mx càng dương thì x trên bản in càng chạy về chiều âm Ox
        mx = -( pos.x + stp->x + pDesign->Width() );
        my = -( pos.y + stp->y + pDesign->Height() );
    }

    // 5.4 với góc 270 độ (-90 độ)
    else if ( angle == -90 )
    {
        mx = -( pos.y + stp->y + pDesign->Width() );
        my = pos.x + stp->x;
    }
    else
    {

    }
    // 6. di chuyển đến vị trí đã tính
    if ( ( mx < -150 ) || ( mx > 150 ) || ( my < -150 ) || ( my > 150 ) )
    {
        QString cmd = tr(
            "Phát hiện tọa độ cần in vượt ngoài khoảng quy định (-150 ~ 150)!\n"
            "Tọa độ: %1,%2\n"
            "Stopper: %3, offset = %4,%5." )
            .arg( mx ).arg( my )
            .arg( stopper ).arg( stp->x ).arg( stp->y );
        throw HxException( cmd );
    }

    items.clear();
    items.push_back( "AG" );
    items.push_back( program );
    items.push_back( QString::number( mx, 'f', 3 ) );
    items.push_back( QString::number( my, 'f', 3 ) );
    items.push_back( "0000.00" );
    items.push_back( "0000" );
    QString cmdMove = items.join( "," ) + "\r";
    try
    {
        fb = SendData( cmdMove, 3000 );
        if ( fb != "AG,0" )
        {
            QString cmd = tr(
                "Dữ liệu phản hồi từ máy khắc laser không đúng!\n"
                "Dữ liệu: %1" )
                .arg( fb );
            throw HxException( cmd );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
    }

    return true;
}

bool HxLaser::Burn()
{
    try
    {
        QString fb = SendData( "NT\r" );
        if ( fb == "NT,0" ) return true;
        else
        {
            QString code = fb.split( ',' ).last();
            throw HxException( tr( "Lệnh in thất bại.\nMã lỗi: %1" ).arg( code ) );
        }
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Laser" );
        throw ex;
    }
    
    return true;
}

void HxLaser::ReLoadSetting()
{
    m_settings.Load();
}

HxLaser* Laser()
{
    static HxLaser instance;
    return &instance;
}
