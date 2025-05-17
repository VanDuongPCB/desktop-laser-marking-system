#pragma once
#include "QObject"
#include "QString"
#include "HxSettings.h"


class HxBarcode : private QObject
{
    Q_OBJECT
public:
    HxBarcode();
    bool IsHasData();
    bool Clear();
    QString Read();
    bool SendFeedback( bool status );
    //void Save( const QString& code );
    void ReLoadSetting();
private:
    HxRegistrySetting m_settings;
};

HxBarcode* Barcode();