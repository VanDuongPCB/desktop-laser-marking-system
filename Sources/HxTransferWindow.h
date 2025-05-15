#pragma once
#include "QMainWindow"

namespace Ui
{
    class TransferWindow;
}

class HxTransferWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxTransferWindow( QWidget* parent = nullptr );
    ~HxTransferWindow();

private:
    Ui::TransferWindow* ui;
    void showEvent( QShowEvent* );
    bool eventFilter( QObject* watched, QEvent* event );
    void OnShowModels();
    void OnSelect( const QString& modelName );
    void OnCvWidthChanged( double width );
    void OnTransfer();
    void OnStop();

    void LockUI();
    void UpdateUI();
};

