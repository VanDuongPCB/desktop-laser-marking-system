#pragma once
#include "stdint.h"
#include <QString>

uint64_t HexStringToUInt64( QString hexStr );
QString UInt64ToHexString( uint64_t value, int length );

