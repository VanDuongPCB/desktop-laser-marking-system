#pragma once
#include "QMainWindow"
#include "HxSettings.h"
#include "HxProfile.h"
#include "HxStopper.h"

namespace Ui
{
    class SettingsWindow;
}

class HxSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxSettingsWindow( QWidget* parent = 0 );
    ~HxSettingsWindow();

private:
    Ui::SettingsWindow* ui;
    HxRegistrySetting m_regSettings;
    HxStopperPtrMap m_pStoppers;

    void showEvent( QShowEvent* );
    bool eventFilter( QObject* watched, QEvent* event );
    void Show();

    void OnLoad();
    void OnBrowseDataDir();
    void OnBrowseIVDir();
    void OnEnumerateLaserPorts();
    void OnSave();
    void OnMigration();
};

