#include "HxModel.h"
#include "HxSettings.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QCoreApplication>


HxModel::HxModel()
{
    comments.insert( "FIX1", "" );
    comments.insert( "FIX2", "" );
    comments.insert( "FIX3", "" );
    comments.insert( "FIX4", "" );
    comments.insert( "FIX5", "" );
}

HxModel::~HxModel()
{

}

QString HxModel::value( QString paramName )
{
    if ( paramName == "CODE" ) return code;
    else if ( paramName == "NAME" ) return name;
    else if ( paramName == "NO" ) return kNo;
    else if ( comments.contains( paramName ) )
    {
        return comments.value( paramName );
    }
    else
    {
        return "MODEL." + paramName;
    }
}

/* -------------------------------------------------- */
std::vector<std::shared_ptr<HxModel>> HxModel::items;


std::shared_ptr<HxModel> HxModel::create( QString name, QString code )
{
    auto ex = find( name );
    if ( ex != nullptr ) return ex;
    ex = std::make_shared<HxModel>( HxModel() );
    ex->name = name;
    ex->code = code;
    items.push_back( ex );
    return ex;
}

void HxModel::addModelsFromFile( QString file )
{
    QFile reader( file );
    QStringList lines;
    if ( reader.open( QIODevice::ReadOnly ) )
    {
        while ( !reader.atEnd() )
        {
            lines.push_back( reader.readLine().trimmed() );
        }
        reader.close();
    }

    if ( lines.size() < 2 ) return;
    QStringList headers;
    QList<QStringList> RowItemData;

    QStringList headerTextItems = lines[ 0 ].split( "," );
    for ( auto& text : headerTextItems )
    {
        QString hd = text.trimmed().replace( ")", "" ).replace( "(", "-" ).toUpper();
        headers.push_back( hd );
    }

    int cnt = lines.size();
    for ( int i = 1; i < cnt; i++ )
    {
        RowItemData.push_back( lines[ i ].split( "," ) );
    }


    for ( int row = 0; row < RowItemData.size(); row++ )
    {
        HxModel tempModel;


        for ( int col = 0; col < headers.size(); col++ )
        {
            QString colHeader = headers[ col ];
            if ( colHeader == ";NO" ) continue;
            QString value = RowItemData[ row ][ col ].trimmed();

            // comments
            tempModel.comments.insert( colHeader, value );

            if ( colHeader == "KISHU" )
            {
                tempModel.name = value;
            }
            else if ( colHeader == "CVWIDTH" )
            {
                tempModel.cvWidth = value.toDouble();
            }
            else if ( colHeader == "CMT" )
            {
                tempModel.kNo = value.trimmed();
            }
            else if ( colHeader == "CMT2" )
            {
                QString ivProgram = value;
                tempModel.ivProgram = ivProgram.replace( "IV:", "" ).trimmed();
            }
            else if ( colHeader == "BUBAN" )
            {
                tempModel.code = value;
            }

            else if ( colHeader.startsWith( "LBLPOSX1-" ) )
            {
                int index = colHeader.replace( "LBLPOSX1-", "" ).toInt();
                while ( index >= tempModel.positions.size() )
                {
                    tempModel.positions.push_back( HxPosition() );
                }
                tempModel.positions[ index ].x = value.toDouble();
            }
            else if ( colHeader.startsWith( "LBLPOSY1-" ) )
            {
                int index = colHeader.replace( "LBLPOSY1-", "" ).toInt();
                while ( index >= tempModel.positions.size() )
                {
                    tempModel.positions.push_back( HxPosition() );
                }
                tempModel.positions[ index ].y = value.toDouble();
            }
            else if ( colHeader.startsWith( "LBLDIR1-" ) )
            {
                int index = colHeader.replace( "LBLDIR1-", "" ).toInt();
                while ( index >= tempModel.positions.size() )
                {
                    tempModel.positions.push_back( HxPosition() );
                }
                tempModel.positions[ index ].angle = value.toInt();
            }
        }
        if ( tempModel.code.length() < 1 || tempModel.name.length() < 1 )
        {
            continue;
        }

        auto targetModel = HxModel::find( tempModel.name );
        if ( targetModel == nullptr )
        {
            targetModel = std::make_shared<HxModel>( tempModel );
            items.push_back( targetModel );
        }
    }
}

void HxModel::save( std::shared_ptr<HxModel> model )
{
    if ( model == nullptr ) return;
    QJsonObject obj;
    // common
    obj.insert( "code", model->code );
    obj.insert( "cv-width", model->cvWidth );
    obj.insert( "stopper", model->stopper );
    obj.insert( "iv-program", model->ivProgram );
    obj.insert( "design", model->design );
    obj.insert( "k-no", model->kNo );
    obj.insert( "laser-power", model->power );

    // pos
    QJsonArray posArr;
    for ( auto& pos : model->positions )
    {
        if ( pos.x == 0 && pos.y == 0 && pos.angle == 0 ) continue;
        QJsonObject posObj;
        posObj.insert( "x", pos.x );
        posObj.insert( "y", pos.y );
        posObj.insert( "angle", pos.angle );
        posArr.push_back( posObj );
    }
    obj.insert( "pos", posArr );

    // comments
    QJsonObject commentObj;
    QStringList keys = model->comments.keys();
    for ( auto& key : keys )
    {
        commentObj.insert( key, model->comments.value( key ) );
    }
    obj.insert( "comments", commentObj );
    QJsonDocument doc;
    doc.setObject( obj );

    QString dir = QCoreApplication::applicationDirPath() + "/data";
    QDir().mkdir( dir );
    dir += "/MODELS";
    QDir().mkdir( dir );
    QString path = dir + "/" + model->name + ".model";
    QFile writer( path );
    if ( writer.open( QIODevice::WriteOnly ) )
    {
        writer.write( doc.toJson() );
        writer.close();
    }
}

void HxModel::save()
{
    for ( auto& item : items )
    {
        save( item );
    }
}

void HxModel::load()
{
    QString dir = QCoreApplication::applicationDirPath() + "/data";

    QDir().mkdir( dir );
    dir += "/MODELS";

    QDir().mkdir( dir );
    QFileInfoList files = QDir( dir ).entryInfoList( { "*.model" } );



    for ( auto& file : files )
    {
        QFile reader( file.absoluteFilePath() );
        if ( reader.open( QIODevice::ReadOnly ) )
        {
            QByteArray json = reader.readAll();
            reader.close();

            QJsonDocument doc = QJsonDocument::fromJson( json );
            QJsonObject obj = doc.object();
            HxModel* model = new HxModel();
            model->code = obj.value( "code" ).toString();
            model->name = file.baseName().toUpper();
            model->kNo = obj.value( "k-no" ).toString();
            model->ivProgram = obj.value( "iv-program" ).toString();
            model->design = obj.value( "design" ).toString();
            model->cvWidth = obj.value( "cv-width" ).toDouble();
            model->stopper = obj.value( "stopper" ).toInt( 1 );
            model->power = obj.value( "laser-power" ).toDouble( 1 );

            // markPositions;
            QJsonArray posArr = obj.value( "pos" ).toArray();
            for ( auto posItem : posArr )
            {
                QJsonObject objPos = posItem.toObject();
                HxPosition pos;
                pos.x = objPos.value( "x" ).toDouble( 0 );
                pos.y = objPos.value( "y" ).toDouble( 0 );
                pos.angle = objPos.value( "angle" ).toDouble( 0 );
                model->positions.push_back( pos );
            }

            // comments;
            QJsonObject commentObj = obj.value( "comments" ).toObject();
            QStringList keys = commentObj.keys();
            for ( auto& key : keys )
            {
                model->comments.insert( key, commentObj.value( key ).toString() );
            }


            items.emplace_back( model );
        }
    }
}

QStringList HxModel::names()
{
    QStringList _names;
    for ( auto& item : items )
    {
        _names.push_back( item->name );
    }
    return _names;
}

std::shared_ptr<HxModel> HxModel::find( QString name )
{
    for ( auto& item : items )
    {
        if ( item->name == name )
        {
            return item;
        }
    }
    return {};
}

QStringList HxModel::paramNames()
{
    QStringList names = { "NAME","CODE","NO","FIX1","FIX2","FIX3","FIX4","FIX5" };
    for ( auto& it : items )
    {
        QStringList keys = it->comments.keys();
        for ( auto& key : keys )
        {
            if ( names.contains( key ) == false )
            {
                names.push_back( key );
            }
        }
    }
    return names;
}
