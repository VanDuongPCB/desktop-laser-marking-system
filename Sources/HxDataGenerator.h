#pragma once

#include "HxDesign.h"
#include "HxModel.h"
#include "HxLOT.h"

QString GenMarkData( const QString& format, HxLOTPtr pLOT, HxModelPtr pModel, int shift = 0 );
std::map<int, QString> GenMarkData( HxDesignPtr pDesign, HxLOTPtr pLOT, HxModelPtr pModel, int shift = 0 );