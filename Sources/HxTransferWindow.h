#pragma once
#include "QMainWindow"
#include "HxMainWindow.h"

namespace Ui
{
    class TransferWindow;
}

class HxTransferWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxTransferWindow( QWidget* parent = 0 );
    ~HxTransferWindow();

private:
    Ui::TransferWindow* ui;
    bool eventFilter( QObject* watched, QEvent* event );
    void OnShowModels();
    void OnSelect( const QString& modelName );
    void OnCvWidthChanged( double width );
    void OnTransfer();
    void OnStop();
    void UpdateUI();
};

