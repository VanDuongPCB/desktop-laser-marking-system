#ifndef IVPROGRAM_H
#define IVPROGRAM_H
#include <vector>
#include <memory>
#include <QString>

class IVProgram{
public:
    QString name;
    QString filePath;
    IVProgram();
    ~IVProgram();


public:
    static std::vector<std::shared_ptr<IVProgram>> items;
    static void load();
    static QStringList names();
};

#endif // IVPROGRAM_H
