#pragma once
#include "QMap"
#include "QString"
#include "memory"
#include "vector"
#include "HxBlockInfo.h"


class HxDesign
{
public:
    QString name = "0000";
    double width = 10;
    double height = 10;
    QMap<int, HxBlockInfo> blocks;
    HxDesign();
    ~HxDesign();
    int indexOfBlockCode();
public:
    static std::vector<std::shared_ptr<HxDesign>> items;
    static std::shared_ptr<HxDesign> find( QString name );
    static void load();
    static void save( std::shared_ptr<HxDesign> design );
    static void save();
};

