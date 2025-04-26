#ifndef CONVERT_H
#define CONVERT_H
#include "stdint.h"
#include <QString>


class Convert{
public:
    static uint64_t uint64FromHexString(QString hexStr);
    static QString hexStringFromUint64(uint64_t value, int length);
};

#endif // CONVERT_H
