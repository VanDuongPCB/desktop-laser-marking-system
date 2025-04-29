#pragma once
#include <vector>
#include "QString"
#include <QMap>
#include <QJsonObject>
#include <vector>
#include <memory>
#include "HxPosition.h"

class HxModel
{
public:
    QString code;
    QString name;
    QString kNo;
    QString ivProgram;
    QString design;
    double cvWidth = 0;
    int stopper = 1;
    std::vector<HxPosition> positions;
    QMap<QString, QString> comments;
    double power = 1;

    QString filePath;
public:
    HxModel();
    ~HxModel();
    QString Value( QString paramName );
public:
    static std::vector<std::shared_ptr<HxModel>> items;
    static std::shared_ptr<HxModel> Create( QString name, QString code );
    static void AddModelsFromFile( QString file );
    static void Save( std::shared_ptr<HxModel> model );
    static void Save();
    static void Load();
    static QStringList Names();
    static std::shared_ptr<HxModel> Find( QString name );

    static QStringList ParamNames();
};
