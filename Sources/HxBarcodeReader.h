#pragma once
#include <QString>

class HxBarcodeReader
{
public:
    static bool hasData();
    static bool clear();
    static QString read();
    static bool sendFeedback( bool status );
};

