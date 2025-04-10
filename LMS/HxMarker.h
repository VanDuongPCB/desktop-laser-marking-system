#pragma once
#include <QObject>
#include <QThread>
#include <atomic>
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
    HxLOTPtr m_pLOT;
    HxModelPtr m_pModel;
    std::shared_ptr<HxDesign> design;
    std::shared_ptr<HxStopper> stopper;

    explicit HxMarker( QObject* parent = 0 );
    ~HxMarker();

    void clear();
    bool Select( HxLOTPtr pLOT );
    bool mark( bool test = false );


    bool isBusy();

    void start();
    void stop();

    void Save( HxLOTPtr pLOT, HxModelPtr pModel, std::shared_ptr<HxDesign> design );
signals:
    void started();
    void printed( std::shared_ptr<HxLOT> lot );
    void stopped();
public slots:
    void task();


    /* ----------- */
private:
    static QThread worker;
    static HxMarker* _instance;
public:
    static void initialize();
    static void terminate();
    static HxMarker* instance();
};

