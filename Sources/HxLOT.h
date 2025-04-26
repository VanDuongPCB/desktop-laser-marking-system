#ifndef LOT_H
#define LOT_H
#include <QString>
#include <QMap>
#include <vector>
#include <memory>

class HxLOT
{
public:
    QString name;
    QString dateCreate;
    QString macStart = "000000000000";
    QString macEnd = "000000000000";
    QString counterStart = "0000";
    int quantity = 1;
    int progress = 0;
    bool isRePrint = false;
    QString modelName;
    QMap<QString, QString> comments;
    int sIndex = 0;

public:
    HxLOT();
    ~HxLOT();
    QString counter();
    QString mac();
    QString status();
    bool nextItem();
    bool isCompleted();
    QString value( QString paramName );
public:
    static std::vector<std::shared_ptr<HxLOT>> items;
    static void sort();
    static void load();
    static void remove( int index );
    static void saveAll();
    static void saveLot( std::shared_ptr<HxLOT> data );
    static std::shared_ptr<HxLOT> create();
    static std::shared_ptr<HxLOT> find( QString name );
    static QStringList paramNames();
};

#endif // LOT_H
