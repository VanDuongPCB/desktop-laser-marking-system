#include "HxModel.h"
#include "QFileInfo"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QStringList"
#include "QDir"
#include "QFile"
#include "QCoreApplication"

#include "HxFileManager.h"

namespace
{
HxModelManager s_instance;
bool IsDiff(const QString& val1, const QString val2)
{
    return val1 != val2;
}

bool IsDiff(int val1, int val2)
{
    return val1 != val2;
}

}
HxModel::HxModel() : HxObject()
{
    m_comments["FIX1"] = "";
    m_comments["FIX2"] = "";
    m_comments["FIX3"] = "";
    m_comments["FIX4"] = "";
    m_comments["FIX5"] = "";
}

HxModel::~HxModel()
{

}

QString HxModel::Code() const
{
    return m_code;
}

QString HxModel::Name() const
{
    return m_name;
}

bool HxModel::IsPrintLo() const
{
    return m_bIsPrintLo;
}

QString HxModel::kNo() const
{
    return m_kNo;
}

QString HxModel::IVProgram() const
{
    return m_ivProgram;
}

QString HxModel::Design() const
{
    return m_design;
}

double HxModel::CvWidth() const
{
    return m_cvWidth;
}

int HxModel::Stopper() const
{
    return m_stopper;
}

std::map<int, HxPosition>& HxModel::Positions()
{
    return m_positions;
}

HxPosition HxModel::Position( int index ) const
{
    auto it = m_positions.find(index);
    if(it != m_positions.end())
        return it->second;
    return HxPosition();
}

std::map<QString, QString>& HxModel::Comments()
{
    return m_comments;
}

QString HxModel::Comment( const QString& name ) const
{
    auto it = m_comments.find(name);
    if(it != m_comments.end())
        return it->second;
    return QString();
}

QString HxModel::Value( QString paramName ) const
{
    if ( paramName == "CODE" ) return m_code;
    else if ( paramName == "NAME" ) return m_name;
    else if ( paramName == "NO" ) return m_kNo;
    else {
        auto it = m_comments.find(paramName);
        if(it != m_comments.end())
            return it->second;
        return "MODEL." + paramName;
    }
}

void HxModel::SetCode( const QString& value )
{
    SetModified(IsDiff(m_code, value));
    m_code = value;
}

void HxModel::SetName( const QString& value )
{
    SetModified(IsDiff(m_name, value));
    m_name = value;
}

void HxModel::SetPrintLo( bool bIsEnable )
{
    SetModified(IsDiff(m_bIsPrintLo, bIsEnable));
    m_bIsPrintLo = bIsEnable;
}

void HxModel::SetkNo( const QString& value )
{
    SetModified(IsDiff(m_kNo, value));
    m_kNo = value;
}

void HxModel::SetIVProgram( const QString& value )
{
    SetModified(IsDiff(m_ivProgram, value));
    m_ivProgram = value;
}

void HxModel::SetDesign( const QString& value )
{
    SetModified(IsDiff(m_design, value));
    m_design = value;
}

void HxModel::SetDesign( size_t value )
{
    SetDesign(QString::number(value).rightJustified(4,'0'));
}

void HxModel::SetCvWidth( double value )
{
    SetModified(IsDiff(m_cvWidth, value));
    m_cvWidth = value;
}

void HxModel::SetStopper( int value )
{
    SetModified(IsDiff(m_stopper, value));
    m_stopper = value;
}

void HxModel::SetPositions( const std::map<int, HxPosition>& value )
{
    SetModified(true);
    m_positions = value;
}

void HxModel::SetPosition( int index, const HxPosition& value )
{
    SetModified(1);
    m_positions[index] = value;
}

void HxModel::RemovePosition( int index )
{
    SetModified(1);
    m_positions.erase(index);
}

void HxModel::SetComments( const std::map<QString, QString>& comments )
{
    m_comments = comments;
    SetModified(1);
}

void HxModel::SetComment( const QString& key, const QString& value )
{
    m_comments[key.trimmed().toUpper()] = value;
    SetModified(1);
}

void HxModel::AddComments( const QStringList& keys)
{
    for(auto &key : keys)
    {
        auto it = m_comments.find(key.trimmed().toUpper());
        if(it == m_comments.end())
        {
            m_comments[key.trimmed().toUpper()] = "";
            SetModified(1);
        }
    }
}

void HxModel::RemoveComments( const QStringList& keys )
{
    for(auto &key : keys)
    {
        auto it = m_comments.find(key);
        if(it != m_comments.end())
        {
            m_comments.erase(key);
            SetModified(1);
        }

        it = m_comments.find(key.trimmed().toUpper());
        if(it != m_comments.end())
        {
            m_comments.erase(key.trimmed().toUpper());
            SetModified(1);
        }
    }
}

void HxModel::SetValue( const QString& key, const QString& value )
{
    m_comments[key.trimmed().toUpper()] = value;
    SetModified(1);
}

/* -------------------------------------------------- */
// std::vector<std::shared_ptr<HxModel>> HxModel::items;


// std::shared_ptr<HxModel> HxModel::create( QString name, QString code )
// {
//     auto ex = find( name );
//     if ( ex != nullptr ) return ex;
//     ex = std::make_shared<HxModel>( HxModel() );
//     ex->m_name = name;
//     ex->m_code = code;
//     items.push_back( ex );
//     return ex;
// }

// void HxModel::save( std::shared_ptr<HxModel> model )
// {
//     if ( model == nullptr ) return;
//     QJsonObject obj;
//     // common
//     obj.insert( "code", model->m_code );
//     obj.insert( "is-print-lo", model->IsPrintLo());
//     obj.insert( "cv-width", model->m_cvWidth );
//     obj.insert( "stopper", model->m_stopper );
//     obj.insert( "iv-program", model->m_ivProgram );
//     obj.insert( "design", model->m_design );
//     obj.insert( "k-no", model->m_kNo );

//     // pos
//     QJsonArray posArr;
//     for ( auto& [index, pos] : model->m_positions )
//     {
//         QJsonObject posObj;
//         posObj.insert( "index", index );
//         posObj.insert( "x", pos.x );
//         posObj.insert( "y", pos.y );
//         posObj.insert( "angle", pos.angle );
//         posArr.push_back( posObj );
//     }
//     obj.insert( "pos", posArr );

//     // comments
//     QJsonObject commentObj;
//     for ( auto& [key, value] : model->m_comments )
//     {
//         commentObj.insert( key, value );
//     }
//     obj.insert( "comments", commentObj );
//     QJsonDocument doc;
//     doc.setObject( obj );

//     QString dir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
//     QDir().mkdir( dir );
//     QString path = dir + "/" + model->m_name + ".model";
//     QFile writer( path );
//     if ( writer.open( QIODevice::WriteOnly ) )
//     {
//         writer.write( doc.toJson() );
//         writer.close();
//     }
// }

// void HxModel::save()
// {
//     for ( auto& item : items )
//     {
//         save( item );
//     }
// }

// void HxModel::load()
// {
//     QString modelDir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
//     QDir().mkdir( modelDir );
//     QFileInfoList files = QDir( modelDir ).entryInfoList( { "*.model" } );

//     for ( auto& file : files )
//     {
//         QFile reader( file.absoluteFilePath() );
//         if ( reader.open( QIODevice::ReadOnly ) )
//         {
//             QByteArray json = reader.readAll();
//             reader.close();

//             QJsonDocument doc = QJsonDocument::fromJson( json );
//             QJsonObject obj = doc.object();
//             HxModel* model = new HxModel();
//             model->m_code = obj.value( "code" ).toString();
//             model->m_name = file.baseName().toUpper();
//             model->m_bIsPrintLo = obj.value("is-print-lo").toBool();
//             model->m_kNo = obj.value( "k-no" ).toString();
//             model->m_ivProgram = obj.value( "iv-program" ).toString();
//             model->m_design = obj.value( "design" ).toString();
//             model->m_cvWidth = obj.value( "cv-width" ).toDouble();
//             model->m_stopper = obj.value( "stopper" ).toInt( 1 );

//             // markPositions;
//             QJsonArray posArr = obj.value( "pos" ).toArray();
//             int posIndex=1;
//             for ( auto posItem : posArr )
//             {
//                 QJsonObject objPos = posItem.toObject();
//                 HxPosition pos;
//                 pos.x = objPos.value( "x" ).toDouble( 0 );
//                 pos.y = objPos.value( "y" ).toDouble( 0 );
//                 pos.angle = objPos.value( "angle" ).toDouble( 0 );
//                 model->m_positions[posIndex++] = pos;
//             }

//             // comments;
//             QJsonObject commentObj = obj.value( "comments" ).toObject();
//             QStringList keys = commentObj.keys();
//             for ( auto& key : keys )
//             {
//                 model->m_comments[ key ] = commentObj.value( key ).toString();
//             }


//             items.emplace_back( model );
//         }
//     }
// }

QStringList HxModel::paramNames()
{
    QStringList names = { "NAME","CODE","NO","FIX1","FIX2","FIX3","FIX4","FIX5" };
    // for ( auto& it : items )
    // {
    //     for ( auto& [key, value] : it->m_comments )
    //     {
    //         if ( names.contains( key ) == false )
    //         {
    //             names.push_back( key );
    //         }
    //     }
    // }
    return names;
}

HxModelPtr HxModelManager::Create( const QString& code, const QString& name )
{
    auto pModel = std::make_shared<HxModel>();
    pModel->SetCode(code);
    pModel->SetName(name);
    return pModel;
}

QStringList HxModelManager::Names()
{
    QStringList items;
    QString modelDir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
    QDir().mkdir( modelDir );
    QFileInfoList files = QDir( modelDir ).entryInfoList( { "*.model" } );
    for ( auto& file : files )
    {
        items.push_back(file.baseName().toUpper());
    }
    return items;
}

QStringList HxModelManager::ParamNames()
{
    QStringList items;
    return items;
}

HxModelPtrMap HxModelManager::GetModels()
{
    HxModelPtrMap map;
    QString modelDir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
    QDir().mkdir( modelDir );
    QFileInfoList files = QDir( modelDir ).entryInfoList( { "*.model" } );

    for ( auto& file : files )
    {
        QFile reader( file.absoluteFilePath() );
        if ( !reader.open( QIODevice::ReadOnly ) )
            continue;

        HxModelPtr pModel = Create();
        QByteArray json = reader.readAll();
        reader.close();

        QJsonDocument doc = QJsonDocument::fromJson( json );
        QJsonObject obj = doc.object();

        pModel->SetCode( obj.value( "code" ).toString() );
        pModel->SetName( file.baseName().toUpper() );
        pModel->SetPrintLo( obj.value("is-print-lo").toBool() );
        pModel->SetkNo( obj.value( "k-no" ).toString() );
        pModel->SetIVProgram( obj.value( "iv-program" ).toString() );
        pModel->SetDesign( obj.value( "design" ).toString() );
        pModel->SetCvWidth( obj.value( "cv-width" ).toDouble() );
        pModel->SetStopper( obj.value( "stopper" ).toInt( 1 ) );

        // markPositions;
        QJsonArray posArr = obj.value( "pos" ).toArray();
        int posIndex=1;
        for ( auto posItem : posArr )
        {
            QJsonObject objPos = posItem.toObject();
            HxPosition pos;
            pos.index = objPos.value("index").toInt();
            if(pos.index<=0)
                pos.index = posIndex;
            pos.x = objPos.value( "x" ).toDouble( 0 );
            pos.y = objPos.value( "y" ).toDouble( 0 );
            pos.angle = objPos.value( "angle" ).toDouble( 0 );

            pModel->SetPosition(pos.index, pos);
            posIndex++;
        }

        // comments;
        QJsonObject commentObj = obj.value( "comments" ).toObject();
        QStringList keys = commentObj.keys();
        for ( auto& key : keys )
        {
            pModel->SetComment( key, commentObj.value( key ).toString());
        }

        pModel->ClearModified();
        QString modeName = file.baseName().toUpper();
        map[modeName] = pModel;
    }
    return map;
}

HxModelPtr HxModelManager::GetModel( const QString& name )
{
    HxModelPtr pModel;
    if(name.isEmpty())
        return pModel;

    QString modelDir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
    QDir().mkdir( modelDir );
    QString modelFile = QString("%1/%2.model").arg(modelDir).arg(name);

    QFile reader( modelFile );
    if ( !reader.open( QIODevice::ReadOnly ) )
        return pModel;

    QByteArray json = reader.readAll();
    reader.close();

    QJsonDocument doc = QJsonDocument::fromJson( json );
    QJsonObject obj = doc.object();

    if( !pModel )
        pModel = Create("", name);

    pModel->SetCode( obj.value( "code" ).toString() );
    pModel->SetName( name.toUpper() );
    pModel->SetPrintLo( obj.value("is-print-lo").toBool() );
    pModel->SetkNo( obj.value( "k-no" ).toString() );
    pModel->SetIVProgram( obj.value( "iv-program" ).toString() );
    pModel->SetDesign( obj.value( "design" ).toString() );
    pModel->SetCvWidth( obj.value( "cv-width" ).toDouble() );
    pModel->SetStopper( obj.value( "stopper" ).toInt( 1 ) );

    // markPositions;
    QJsonArray posArr = obj.value( "pos" ).toArray();
    int posIndex=1;
    for ( auto posItem : posArr )
    {
        QJsonObject objPos = posItem.toObject();
        HxPosition pos;
        pos.index = objPos.value("index").toInt();
        if(pos.index<=0)
            pos.index = posIndex;
        pos.x = objPos.value( "x" ).toDouble( 0 );
        pos.y = objPos.value( "y" ).toDouble( 0 );
        pos.angle = objPos.value( "angle" ).toDouble( 0 );

        pModel->SetPosition(pos.index, pos);
        posIndex++;
    }

    // comments;
    QJsonObject commentObj = obj.value( "comments" ).toObject();
    QStringList keys = commentObj.keys();
    for ( auto& key : keys )
    {
        pModel->SetComment( key, commentObj.value( key ).toString());
    }

    pModel->ClearModified();
    return pModel;
}

void HxModelManager::Save( HxModelPtr pModel )
{
    if ( !pModel )
        return;
    QJsonObject obj;
    // common
    obj.insert( "code", pModel->Code() );
    obj.insert( "is-print-lo", pModel->IsPrintLo());
    obj.insert( "cv-width", pModel->CvWidth() );
    obj.insert( "stopper", pModel->Stopper() );
    obj.insert( "iv-program", pModel->IVProgram() );
    obj.insert( "design", pModel->Design() );
    obj.insert( "k-no", pModel->kNo() );

    // pos
    QJsonArray posArr;
    auto positions = pModel->Positions();
    for ( auto& [index, pos] : positions )
    {
        QJsonObject posObj;
        posObj.insert( "index", index );
        posObj.insert( "x", pos.x );
        posObj.insert( "y", pos.y );
        posObj.insert( "angle", pos.angle );
        posArr.push_back( posObj );
    }
    obj.insert( "pos", posArr );

    // comments
    QJsonObject commentObj;
    auto comments = pModel->Comments();
    for ( auto& [key, value] : comments )
    {
        commentObj.insert( key, value );
    }
    obj.insert( "comments", commentObj );
    QJsonDocument doc;
    doc.setObject( obj );

    QString dir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
    QDir().mkdir( dir );
    QString path = dir + "/" + pModel->Name() + ".model";
    QFile writer( path );
    if ( writer.open( QIODevice::WriteOnly ) )
    {
        writer.write( doc.toJson() );
        writer.close();
    }

    pModel->ClearModified();
}

void HxModelManager::Removes( const QStringList& names )
{
    for(auto &name : names )
    {
        QString dir = GetFileManager()->GetPath(HxFileManager::eDBModelDir);
        QDir().mkdir( dir );
        QString path = dir + "/" + name + ".model";
        QFile::remove(path);
    }
}

void HxModelManager::AddComments( const QStringList& keys )
{

}

void HxModelManager::RemoveComments( const QStringList& keys )
{

}


HxModelManager* GetModelManager()
{
    return &s_instance;
}
