#pragma once
#include "map"

#include "QString"

#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"

QString BlockDataGen( QString format, std::shared_ptr<HxLOT> pLOT, std::shared_ptr<HxModel> pModel );
std::map<int, QString> BlockDataGen( std::shared_ptr<HxDesign> pDesign, std::shared_ptr<HxLOT> pLOT, std::shared_ptr<HxModel> pModel );