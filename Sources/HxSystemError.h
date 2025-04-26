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
    void errorReport( HxException ex );

signals:
    void reported( HxException ex );

public:
    static HxSystemError* instance();
};
