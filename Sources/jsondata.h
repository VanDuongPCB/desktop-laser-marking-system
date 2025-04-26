#ifndef JSONDATA_H
#define JSONDATA_H

#include <QByteArray>
#include <QJsonObject>
class JsonData
{
public:
    JsonData();
    ~JsonData();
    virtual QJsonObject toJson() = 0;
};

#endif // JSONDATA_H
