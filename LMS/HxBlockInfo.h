#pragma once
#include "QString"

class HxLOT;
class HxModel;
class HxDesign;


class HxBlockInfo
{
public:
    bool isCode = false;
    QString data;
    int textLen = 0;

    HxBlockInfo();
    ~HxBlockInfo();

    static QString gen( QString format, std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model );
    static QMap<int, QString> gen( std::shared_ptr<HxDesign> design, std::shared_ptr<HxLOT> lot, std::shared_ptr<HxModel> model );
};

