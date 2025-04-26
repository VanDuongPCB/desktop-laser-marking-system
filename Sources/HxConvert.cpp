#include "HxConvert.h"


uint64_t HexStringToUInt64( QString hexStr )
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

QString UInt64ToHexString( uint64_t value, int length )
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
