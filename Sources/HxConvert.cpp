#include "HxConvert.h"


uint64_t Uint64FromHexString( const QString& hexStr )
{
    std::string s = hexStr.trimmed().toUpper().toStdString();
    uint64_t val = 0;
    int size = s.length();
    for ( int i = 0; i < size; i++ )
    {
        val = val << 4;
        unsigned char c = s[ i ];
        if ( c >= '0' && c <= '9' )
        {
            val |= ( c - '0' );
        }
        else if ( c >= 'A' && c <= 'F' )
        {
            val |= ( 10 + c - 'A' );
        }
    }
    return val;
}

QString HexStringFromUint64( uint64_t value, int length )
{
    std::string base = QString::number( 0 ).rightJustified( length, '0' ).toStdString();
    for ( int i = 0; i < length; i++ )
    {
        uint8_t c = value & 0xF;
        if ( c >= 0 && c <= 9 ) base[ length - i - 1 ] = c + '0';
        else base[ length - i - 1 ] = c - 10 + 'A';
        value = value >> 4;
    }
    return QString::fromStdString( base );
}

QString ProductStatusToString( HxLOT::ProductStatus status )
{
    switch ( status )
    {
    case HxLOT::ePending:
        return "Chưa sản xuất";
    case HxLOT::eProduct:
        return "Đang sản xuất";
    case HxLOT::eCompleted:
        return "Đã hoàn thành";
    default:
        break;
    }
    return "?";
}

QColor ProductStatusToColor( HxLOT::ProductStatus status )
{
    switch ( status )
    {
    case HxLOT::ePending:
        return QColor( 255, 255, 255 );
    case HxLOT::eProduct:
        return QColor( 255, 255, 128 );
    case HxLOT::eCompleted:
        return QColor( 128, 255, 128 );
    default:
        break;
    }
    return QColor( 255, 255, 255 );
}
