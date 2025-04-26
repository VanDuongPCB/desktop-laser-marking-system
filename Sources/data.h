#ifndef DATA_H
#define DATA_H
#include <vector>
#include <memory>
#include <QFileSystemWatcher>
#include <QMutex>

#include "lot.h"
#include "model.h"
#include "design.h"

class Data{
private:
    static Lot* _lot;
    static Model* _model;
    static QMutex mutex;
public:
    static Lot* lot();
    static Model* model();




    static void clear();
    static void select(QString lotName);
    static void lock();
    static void unlock();
};

#endif // DATA_H
