#pragma once
#include "stdint.h"
#include "QString"
#include "QColor"
#include "HxLOT.h"


uint64_t Uint64FromHexString( const QString& hexStr );
QString HexStringFromUint64( uint64_t value, int length );

QString ProductStatusToString(HxLOT::ProductStatus status);
QColor ProductStatusToColor(HxLOT::ProductStatus status);
