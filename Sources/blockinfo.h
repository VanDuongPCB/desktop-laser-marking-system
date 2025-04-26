#ifndef BLOCKINFO_H
#define BLOCKINFO_H
#include <QString>

class Lot;
class Model;
class Design;


class BlockInfo{
public:
    bool isCode = false;
    QString data;
    int textLen = 0;

    BlockInfo();
    ~BlockInfo();

    static QString gen(QString format, std::shared_ptr<Lot> lot, std::shared_ptr<Model> model);
    static QMap<int, QString> gen(std::shared_ptr<Design> design, std::shared_ptr<Lot> lot, std::shared_ptr<Model> model);
};

#endif // BLOCKINFO_H
