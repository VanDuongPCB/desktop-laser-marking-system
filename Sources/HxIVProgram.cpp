#include "HxIVProgram.h"
#include "HxSettings.h"
#include "QDir"
#include "QFileInfo"
#include "QFileInfoList"
#include "QCoreApplication"

#include "HxFileManager.h"

HxIVProgram::HxIVProgram()
{

}

HxIVProgram::~HxIVProgram()
{

}

std::vector<std::shared_ptr<HxIVProgram>> HxIVProgram::items;
void HxIVProgram::load()
{
    //QString ivDir = GetFileManager()->GetPath(HxFileManager::eDBIVProgramDir);
    //QDir().mkdir( ivDir );
    //QFileInfoList files = QDir( ivDir ).entryInfoList( { "*.iva" } );
    //for ( auto& file : files )
    //{
    //    HxIVProgram* program = new HxIVProgram();
    //    program->name = file.baseName().toUpper();
    //    program->filePath = file.absoluteFilePath();
    //    items.emplace_back( program );
    //}
}

QStringList HxIVProgram::names()
{
    QStringList _names;
    //QString ivDir = GetFileManager()->GetPath(HxFileManager::eDBIVProgramDir);
    //QDir().mkdir( ivDir );
    //QFileInfoList files = QDir( ivDir ).entryInfoList( { "*.iva" } );
    //for ( auto& file : files )
    //{
    //    _names.push_back( file.baseName().toUpper() );
    //}

    return _names;
}
