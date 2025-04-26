#pragma once
#include <QMainWindow>

namespace Ui
{
    class SettingWindow;
}

class HxSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxSettingsWindow( QWidget* parent = 0 );
    ~HxSettingsWindow();

private slots:
    void on_btnBrowseDataDir_clicked();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_btnAddUser_clicked();

    void on_tbnRemoveUser_clicked();

    void on_btnResetUser_clicked();

    void on_btnEnum_clicked();

private:
    Ui::SettingWindow* ui;
    void showEvent( QShowEvent* );
    void showProtectSetting();
    void showActuatorSettings();
    void showLaserSettings();
    void showStoppers();
    void showUsers();
};
