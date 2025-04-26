#pragma once
#include <QMap>
#include <QString>
#include <memory>
#include <vector>
#include "HxBlock.h"


class HxDesign
{
public:
    QString name = "0000";
    double width = 10;
    double height = 10;
    QMap<int, HxBlock> blocks;
    HxDesign();
    ~HxDesign();
    int IndexOfBlockCode();
public:
    static std::vector<std::shared_ptr<HxDesign>> items;
    static std::shared_ptr<HxDesign> Find( QString name );
    static void Load();
    static void Save( std::shared_ptr<HxDesign> design );
    static void Save();
};

