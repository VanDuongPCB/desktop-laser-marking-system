#pragma once
#include <stdint.h>
#include <QString>


uint64_t Uint64FromHexString( const QString& hexStr );
QString HexStringFromUint64( uint64_t value, int length );

