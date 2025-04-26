#pragma once
#include "QObject"
#include "QThread"
#include "atomic"
#include "HxLOT.h"
#include "HxDesign.h"
#include "HxModel.h"
#include "HxStopper.h"




class HxMarker : public QObject
{
    Q_OBJECT
private:
    std::atomic_bool runFlag = false;
public:
    std::shared_ptr<HxLOT> lot;
    std::shared_ptr<HxModel> model;
    std::shared_ptr<HxDesign> design;
    std::shared_ptr<HxStopper> stopper;

    explicit HxMarker( QObject* parent = 0 );
    ~HxMarker();

    void Clear();
    bool Select( std::shared_ptr<HxLOT> lot );
    bool Mark( bool test = false );


    bool IsBusy();

    void Start();
    void Stop();
signals:
    void started();
    void printed( std::shared_ptr<HxLOT> lot );
    void stopped();
public slots:
    void Task();


    /* ----------- */
private:
    static QThread worker;
    static HxMarker* _instance;
public:
    static void Initialize();
    static void Terminate();
    static HxMarker* Instance();
};
