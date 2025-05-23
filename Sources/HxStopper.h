#pragma once
#include "QString"
#include "QStringList"
#include "QMap"
#include "vector"
#include "memory"

#include "HxDefines.h"
#include "HxSettings.h"


struct HxStopper;
using HxStopperPtr = std::shared_ptr<HxStopper>;
using HxStopperPtrMap = std::map<int, HxStopperPtr>;

struct HxStopper
{
    double x = -150.0;
    double y = -127.0;
};

class HxStopperManager : private QObject
{
    Q_OBJECT
public:
    HxStopperManager();
    HxStopperPtr Create();
    HxStopperPtr GetStopper( int index );
    HxStopperPtrMap GetStoppers();
    ReturnCode Save( int index, HxStopperPtr pStopper );
    void ReLoadSetting();
private:
    std::map<int, HxStopperPtr> m_items;
    HxRegistrySetting m_settings;
};

HxStopperManager* StopperManager();