#ifndef LASER_H
#define LASER_H
#include <QString>
#include <QMap>
#include "position.h"
#include "design.h"
#include "stopper.h"

class Laser{
private:
    static bool detectPortExisting();
    static QString sendData(QString data, int timeout = 30000);
    static QString detectError(QString data);

public:
    static bool setProgram(QString name);
    static bool setupBlockData(QString program, QMap<int, QString> data);
    static bool setupPosition(QString program, Position pos, int stopper, Design design);
    static bool burn();
};

#endif // LASER_H
