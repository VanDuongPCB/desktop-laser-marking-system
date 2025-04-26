#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <QString>
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
    QString value( QString paramName );
public:
    static std::vector<std::shared_ptr<HxModel>> items;
    static std::shared_ptr<HxModel> create( QString name, QString code );
    static void addModelsFromFile( QString file );
    static void save( std::shared_ptr<HxModel> model );
    static void save();
    static void load();
    static QStringList names();
    static std::shared_ptr<HxModel> find( QString name );

    static QStringList paramNames();
};

#endif // MODEL_H
