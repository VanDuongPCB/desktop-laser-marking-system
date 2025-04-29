#pragma once
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"

class HxLogSaver
{
public:
    static void Save( std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model, std::shared_ptr<HxDesign> design );
};
