#pragma once
#include "QObject"
#include "QThread"
#include "atomic"
#include "HxLOT.h"
#include "HxDesign.h"
#include "HxModel.h"
#include "HxStopper.h"
#include "HxEvent.h"



class HxMarker : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        eOnUnInit,
        eOnFree,
        eOnTesting,
        eOnTransfering,
        eOnMarking,
        eOnFinish,
        eOnStopping,
        eOnError
    };

    enum SetupMode
    {
        eModeTransfer,
        eModeMarking
    };

    HxMarker();
    ~HxMarker();
    State GetState() const;
    void Init();
    void DeInit();
    bool Setup( SetupMode mode, const QString& param );
    void Mark();
    void Transfer();
    void Test();
    void Pause();
    void Stop();
    HxLOTPtr LOT() const;
    HxModelPtr Model() const;
    HxDesignPtr Design() const;
    void ReLoadSetting();
private:
    HxLOTPtr m_pLOT;
    HxModelPtr m_pModel;
    HxDesignPtr m_pDesign;
    HxStopperPtr m_pStopper;
    State m_state = eOnUnInit;

    void CheckAndPostEvent( State& lastState, State currentState, HxEvent::Type eventType );
    void Task();

private:
    HxRegistrySetting m_settings;
};


HxMarker* Marker();