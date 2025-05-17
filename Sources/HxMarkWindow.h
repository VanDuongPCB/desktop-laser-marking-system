#pragma once
#include "QMainWindow"

#include "HxLOT.h"
#include "HxException.h"

namespace Ui
{
    class MarkWindow;
}

class HxMarkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxMarkWindow( QWidget* parent = 0 );
    ~HxMarkWindow();

private:
    Ui::MarkWindow* ui;
    std::vector<HxException> exceptions;
    void showEvent( QShowEvent* );
    bool eventFilter( QObject* watched, QEvent* event );

    void ShowLotInfo();
    void ShowLotStatus();
    void ShowLotBlocks();
    void ShowExceptions();

    void LockUI();
    void UpdateUI();

    void OnException( QJsonObject exData );

    void OnSelect();
    void OnTest();
    void OnRun();
    void OnStop();
};

