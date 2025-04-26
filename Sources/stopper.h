#ifndef STOPPER_H
#define STOPPER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>
#include <memory>

class Stopper{
public:
    double x = -150;
    double y = -127;
    Stopper();
    ~Stopper();

public:
    static QMap<int, std::shared_ptr<Stopper>> items;
    static void load();
    static void save();
    static std::shared_ptr<Stopper> find(int index);
};

#endif // STOPPER_H
