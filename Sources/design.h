#ifndef DESIGN_H
#define DESIGN_H
#include <QMap>
#include <QString>
#include <memory>
#include <vector>
#include "blockinfo.h"


class Design{
public:
    QString name = "0000";
    double width = 10;
    double height = 10;
    QMap<int,BlockInfo> blocks;
    Design();
    ~Design();
    int indexOfBlockCode();
public:
    static std::vector<std::shared_ptr<Design>> items;
    static std::shared_ptr<Design> find(QString name);
    static void load();
    static void save(std::shared_ptr<Design> design);
    static void save();
};

#endif // DESIGN_H
