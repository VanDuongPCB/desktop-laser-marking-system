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
    HxMainWindow* mainWindow = nullptr;
    std::vector<HxException> exceptions;
    void showEvent( QShowEvent* );
    void showLots();
    void showLotInfo( std::shared_ptr<HxLOT> lot );
    void showLotStatus( std::shared_ptr<HxLOT> lot );
    void showLotBlocks();
    void showExceptions();
    void updateUI();
};

