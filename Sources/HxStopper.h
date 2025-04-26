#ifndef STOPPER_H
#define STOPPER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>
#include <memory>

class HxStopper
{
public:
    double x = -150;
    double y = -127;
    HxStopper();
    ~HxStopper();

public:
    static QMap<int, std::shared_ptr<HxStopper>> items;
    static void load();
    static void save();
    static std::shared_ptr<HxStopper> find( int index );
};

#endif // STOPPER_H
