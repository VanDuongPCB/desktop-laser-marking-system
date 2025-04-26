#include "ivprogram.h"
#include "settings.h"
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QCoreApplication>

IVProgram::IVProgram()
{

}

IVProgram::~IVProgram()
{

}

std::vector<std::shared_ptr<IVProgram>> IVProgram::items;
void IVProgram::load(){
    QString ivDir = QCoreApplication::applicationDirPath() + "/data/IV-PROGRAMS";
    QDir().mkdir(ivDir);
    QFileInfoList files = QDir(ivDir).entryInfoList({"*.iva"});
    for(auto &file :files){
        IVProgram *program = new IVProgram();
        program->name = file.baseName().toUpper();
        program->filePath = file.absoluteFilePath();
        items.emplace_back(program);
    }
}

QStringList IVProgram::names(){
    QStringList _names;
    QString ivDir = QCoreApplication::applicationDirPath() + "/data/IV-PROGRAMS";
    QDir().mkdir(ivDir);
    QFileInfoList files = QDir(ivDir).entryInfoList({"*.iva"});
    for(auto &file :files){
        _names.push_back(file.baseName().toUpper());
    }

    return _names;
}
