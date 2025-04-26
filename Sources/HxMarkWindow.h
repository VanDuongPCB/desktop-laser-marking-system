#pragma once
#include <QMainWindow>
#include "HxMainWindow.h"

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
    void HandleException( HxException ex );
    void MarkStarted();
    void MarkStopped();

    void ControllerPrinted( std::shared_ptr<HxLOT> lot );

    void on_actionSelect_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionMark_triggered();

    void on_tbvSelector_doubleClicked( const QModelIndex& index );

    void on_actionLoad_triggered();

private:
    Ui::MarkWindow* ui;
    HxMainWindow* mainWindow = nullptr;
    std::vector<HxException> exceptions;
    void showEvent( QShowEvent* );
    void ShowLots();
    void ShowLotInfo( std::shared_ptr<HxLOT> lot );
    void ShowLotStatus( std::shared_ptr<HxLOT> lot );
    void ShowLotBlocks();
    void ShowExceptions();
    void UpdateUI();
};

