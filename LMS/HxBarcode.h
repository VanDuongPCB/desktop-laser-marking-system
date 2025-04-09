#pragma once
#include <QString>



class HxBarcode
{
public:
    bool hasData();
    bool clear();
    QString read();
    bool sendFeedback( bool status );
    void save( QString code );
};

HxBarcode* GetBarcode();