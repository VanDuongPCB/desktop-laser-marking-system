#ifndef MARKER_H
#define MARKER_H

#include "QObject"
#include "QThread"
#include "atomic"
#include "lot.h"
#include "design.h"
#include "model.h"
#include "stopper.h"




class Marker : public QObject{
    Q_OBJECT
private:
    std::atomic_bool runFlag = false;
public:
    std::shared_ptr<Lot> lot;
    std::shared_ptr<Model> model;
    std::shared_ptr<Design> design;
    std::shared_ptr<Stopper> stopper;

    explicit Marker(QObject *parent = 0);
    ~Marker();

    void clear();
    bool select(std::shared_ptr<Lot> lot);
    bool mark(bool test = false);


    bool isBusy();

    void start();
    void stop();
signals:
    void started();
    void printed(std::shared_ptr<Lot> lot);
    void stopped();
public slots:
    void task();


    /* ----------- */
private:
    static QThread worker;
    static Marker *_instance;
public:
    static void initialize();
    static void terminate();
    static Marker *instance();
};

#endif // MARKER_H
