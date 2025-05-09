#pragma once
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"
#include "HxException.h"

class HxLogger
{
public:
    void Save( std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model, std::shared_ptr<HxDesign> design );

    void SavePrint( std::map<int, QString>& data, int blockCodeIndex, const QString& lotName, const QString& modelName, const QString& designName );
    void SaveRePrint( std::map<int, QString>& data, int blockCodeIndex, const QString& lotName, const QString& modelName, const QString& designName );
    void SaveError( HxException& ex );
};


HxLogger* Logger();