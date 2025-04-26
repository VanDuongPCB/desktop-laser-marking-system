#pragma once
#include <QString>
#include <QMap>
#include "QObject"
#include "HxException.h"

class HxSystemError : public QObject
{
    Q_OBJECT

public:
    HxSystemError();
    void ErrorReport( HxException ex );

signals:
    void Reported( HxException ex );

public:
    static HxSystemError* Instance();
};
