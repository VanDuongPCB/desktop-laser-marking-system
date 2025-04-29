#include "HxDataGenerator.h"

QString BlockDataGen( QString format, std::shared_ptr<HxLOT> pLOT, std::shared_ptr<HxModel> pModel )
{
    if ( format.isEmpty() ) 
        return "";
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
                    QString value = pModel->Value( paramName );
                    itemDatas.push_back( value );
                }
                else if ( objectName == "lot" )
                {
                    QString value = pLOT->Value( paramName );
                    itemDatas.push_back( value );
                }
            }
        }
    }
    QString data = itemDatas.join( ',' );
    return data.replace( ",", "" );
}


std::map<int, QString> BlockDataGen( std::shared_ptr<HxDesign> pDesign, std::shared_ptr<HxLOT> pLOT, std::shared_ptr<HxModel> pModel )
{
    std::map<int, QString> items;
    if ( !pDesign || !pModel || !pLOT ) 
        return items;

    QList<int> nums = pDesign->blocks.keys();
    std::sort( nums.begin(), nums.end() );
    for ( auto num : nums )
    {
        if ( num < 1 ) continue;
        HxBlock block = pDesign->blocks[ num ];
        QString format = block.data.trimmed();
        if ( format.length() < 1 ) continue;
        QString data = BlockDataGen( format, pLOT, pModel );
        items[ num ] = data.replace( ",", "" );
    }
    return items;
}
