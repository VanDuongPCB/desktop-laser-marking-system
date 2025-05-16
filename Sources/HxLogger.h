#pragma once
#include "HxDefines.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"
#include "HxException.h"
#include "HxSettings.h"

struct HxLog
{
    QString Time;
    QString Serial;
    QString LOT;
    QString Model;
    QStringList items;
};

using HxLogArray = std::vector<HxLog>;

class HxLogger : private QObject
{
    Q_OBJECT
public:
    HxLogger();
    ReturnCode CheckSerialExisting( const QString& serial );
    ReturnCode SavePrint( HxLOTPtr pLOT, HxModelPtr pModel, HxDesignPtr pDesign, std::map<int, QString>& blockdata );
    ReturnCode SaveError( const QString& time, const QString& where, const QString& message );
    ReturnCode SaveBarcode( const QString& code );

    ReturnCode Get( QDate fromDate, QDate toDate, HxLogArray& items );
    ReturnCode Get( const QString& serial, HxLogArray& items );

    void Export( HxLogArray& items, QDate fromDate, QDate toDate );
    void ReLoadSetting();
private:
    HxRegistrySetting m_settings;
};


HxLogger* Logger();