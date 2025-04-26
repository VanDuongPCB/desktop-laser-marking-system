#pragma once
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
    QString Counter();
    QString MAC();
    QString Status();
    bool NextItem();
    bool IsCompleted();
    QString Value( QString paramName );
public:
    static std::vector<std::shared_ptr<HxLOT>> items;
    static void Sort();
    static void Load();
    static void Remove( int index );
    static void SaveAll();
    static void SaveLot( std::shared_ptr<HxLOT> data );
    static std::shared_ptr<HxLOT> Create();
    static std::shared_ptr<HxLOT> Find( QString name );
    static QStringList ParamNames();
};
