#pragma once
#include "QString"
#include <QStringList>
#include <QMap>
#include <vector>
#include <memory>

class HxStopper
{
public:
    double x = -150;
    double y = -127;
    HxStopper();
    ~HxStopper();

public:
    static QMap<int, std::shared_ptr<HxStopper>> items;
    static void Load();
    static void Save();
    static std::shared_ptr<HxStopper> Find( int index );
};
