#pragma once
#include "QMainWindow"
#include "HxMainWindow.h"

namespace Ui
{
    class PassWindow;
}

class HxPassWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxPassWindow( QWidget* parent = 0 );
    ~HxPassWindow();

private slots:
    void passStarted();
    void passStopped();

    void on_btnPass_clicked();

    void on_btnReload_clicked();

    void on_spxCvWidth_valueChanged( double arg1 );

    void on_cbxModel_currentTextChanged( const QString& arg1 );

    void on_btnStop_clicked();

private:
    Ui::PassWindow* ui;
    HxMainWindow* mainWindow = nullptr;
};

