#pragma once
#include <QString>

class HxBarcodeReader
{
public:
    static bool IsHasData();
    static bool Clear();
    static QString Read();
    static bool SendFeedback( bool status );
};

