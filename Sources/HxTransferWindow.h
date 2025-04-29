#pragma once
#include <QMainWindow>
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

private slots:
    void PassStarted();
    void PassStopped();

    void on_btnPass_clicked();

    void on_btnReload_clicked();

    void on_spxCvWidth_valueChanged( double arg1 );

    void on_cbxModel_currentTextChanged( const QString& arg1 );

    void on_btnStop_clicked();

private:
    Ui::TransferWindow* ui;
    HxMainWindow* mainWindow = nullptr;
};
