#pragma once
#include <vector>
#include <memory>
#include <QString>

class HxIVProgram
{
public:
    QString name;
    QString filePath;
    HxIVProgram();
    ~HxIVProgram();


public:
    static std::vector<std::shared_ptr<HxIVProgram>> items;
    static void Load();
    static QStringList Names();
};

