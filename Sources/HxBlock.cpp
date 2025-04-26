#include "HxBlock.h"
#include "HxModel.h"
#include "HxLOT.h"
#include "HxDesign.h"
#include <QStringList>
#include <QString>
#include <QDateTime>


QString HxBlock::gen( QString format, std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model )
{
    if ( format.length() < 1 ) return "";
    QStringList formatItems = format.split( ',' );
    QStringList itemDatas;
    for ( auto& formatItem : formatItems )
    {
        QStringList itemParams = formatItem.split( '.' );
        if ( itemParams.size() != 2 )
        {
            itemDatas.push_back( formatItem );
        }
        else
        {
            if ( itemParams[ 0 ].trimmed().length() < 1 || itemParams[ 1 ].trimmed().length() < 1 )
            {
                itemDatas.push_back( formatItem );
            }
            QString objectName = itemParams[ 0 ].trimmed().toLower();
            if ( objectName == "time" )
            {
                itemDatas.push_back( QDateTime::currentDateTime().toString( itemParams[ 1 ] ) );
            }
            else
            {
                QString paramName = itemParams[ 1 ].toUpper().trimmed();
                if ( objectName == "model" )
                {
                    QString value = model->value( paramName );
                    itemDatas.push_back( value );
                }
                else if ( objectName == "lot" )
                {
                    QString value = lot->value( paramName );
                    itemDatas.push_back( value );
                }
            }
        }
    }
    QString data = itemDatas.join( ',' );
    return data.replace( ",", "" );
}


QMap<int, QString> HxBlock::gen( std::shared_ptr<HxDesign> design, std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model )
{
    QMap<int, QString> items;
    if ( design == nullptr ) return items;
    if ( model == nullptr ) return items;
    QList<int> nums = design->blocks.keys();
    std::sort( nums.begin(), nums.end() );
    for ( auto num : nums )
    {
        if ( num < 1 ) continue;
        HxBlock block = design->blocks[ num ];
        QString format = block.data.trimmed();
        if ( format.length() < 1 ) continue;
        QString data = gen( format, lot, model );
        items.insert( num, data.replace( ",", "" ) );
    }
    return items;
}
