#pragma once
#include "QString"
#include "QMap"
#include "QObject"
#include "HxException.h"

class HxSystemReport : public QObject
{
    Q_OBJECT

public:
    HxSystemReport();
    void Report( HxException ex );

signals:
    void reported( HxException ex );
};

HxSystemReport* GetSystemReport();