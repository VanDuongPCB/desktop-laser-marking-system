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

private slots:
    void handleException( HxException ex );
    void markStarted();
    void markStopped();

    void controllerPrinted( std::shared_ptr<HxLOT> lot );

    void on_actionSelect_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionMark_triggered();

    void on_tbvSelector_doubleClicked( const QModelIndex& index );

    void on_actionLoad_triggered();

private:
    Ui::MarkWindow* ui;
    std::vector<HxException> exceptions;
    void showEvent( QShowEvent* );
    bool eventFilter( QObject* watched, QEvent* event );

    void ShowLotInfo();
    void ShowLotStatus();
    void ShowLotBlocks();
    void ShowExceptions();
    void UpdateUI();

    void OnSelect();
    void OnTest();
    void OnRun();
    void OnStop();
};

