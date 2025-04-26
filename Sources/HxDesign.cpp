#include "HxDesign.h"
#include "HxSettings.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>


HxDesign::HxDesign()
{

}

HxDesign::~HxDesign()
{

}

int HxDesign::IndexOfBlockCode()
{
    int index = 1;
    QList<int> keys = blocks.keys();
    for ( auto it : keys )
    {
        if ( blocks[ it ].isCode )
        {
            index = it;
            break;
        }
    }
    return index;
}


/* --------------------------------------------------------------- */

std::vector<std::shared_ptr<HxDesign>> HxDesign::items;


std::shared_ptr<HxDesign> HxDesign::Find( QString name )
{
    for ( auto& item : items )
    {
        if ( item->name != name ) continue;
        return item;
    }
    return {};
}

void HxDesign::Load()
{
    items.clear();

    QString designDir = QCoreApplication::applicationDirPath() + "/data/DESIGNS";
    QDir().mkdir( designDir );

    for ( int i = 0; i < 2000; i++ )
    {
        QString designName = QString::number( i ).rightJustified( 4, '0' );
        HxDesign* design = new HxDesign();
        items.emplace_back( design );
        design->name = designName;
        QString designPath = designDir + "/" + designName + ".design";
        QFile fileReader( designPath );
        if ( fileReader.open( QIODevice::ReadOnly ) )
        {
            QByteArray json = fileReader.readAll();
            fileReader.close();
            QJsonDocument doc = QJsonDocument::fromJson( json );
            QJsonObject obj = doc.object();
            design->width = obj.value( "width" ).toDouble( 5 );
            design->height = obj.value( "height" ).toDouble( 5 );
            QJsonArray arr = obj.value( "blocks" ).toArray();
            for ( auto arrItem : arr )
            {
                QJsonObject objBlock = arrItem.toObject();
                int index = objBlock.value( "index" ).toInt( 0 );
                if ( index < 1 ) continue;
                if ( !design->blocks.contains( index ) )
                {
                    design->blocks.insert( index, HxBlock() );
                }
                design->blocks[ index ].isCode = objBlock.value( "is-code" ).toBool( false );
                design->blocks[ index ].data = objBlock.value( "data" ).toString();
                design->blocks[ index ].textLen = objBlock.value( "text-length" ).toInt( 1 );
            }
        }
    }
}

void HxDesign::Save( std::shared_ptr<HxDesign> design )
{
    QString designDir = QCoreApplication::applicationDirPath() + "/data/DESIGNS";
    QDir().mkdir( designDir );

    QJsonObject objDesign;
    objDesign.insert( "width", design->width );
    objDesign.insert( "height", design->height );


    QList<int> blockNums = design->blocks.keys();
    QJsonArray arrBlocks;
    for ( auto idx : blockNums )
    {
        if ( idx < 1 ) continue;
        HxBlock block = design->blocks[ idx ];
        if ( block.data.trimmed().length() == 0 ) continue;
        QJsonObject objBlock;
        objBlock.insert( "index", idx );
        objBlock.insert( "is-code", block.isCode );
        objBlock.insert( "data", block.data );
        objBlock.insert( "text-length", block.textLen );
        arrBlocks.push_back( objBlock );
    }

    objDesign.insert( "blocks", arrBlocks );
    QJsonDocument doc;
    doc.setObject( objDesign );

    QString designPath = designDir + "/" + design->name + ".design";
    QFile fileWriter( designPath );
    if ( fileWriter.open( QIODevice::WriteOnly ) )
    {
        fileWriter.write( doc.toJson() );
        fileWriter.close();
    }
}

void HxDesign::Save()
{
    QString designDir = QCoreApplication::applicationDirPath() + "/data/DESIGNS";
    QDir().mkdir( designDir );

    for ( auto& item : items )
    {
        Save( item );
    }
}
