#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:
    void on_btnBrowseDataDir_clicked();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_btnAddUser_clicked();

    void on_tbnRemoveUser_clicked();

    void on_btnResetUser_clicked();

    void on_btnEnum_clicked();

private:
    Ui::SettingsWindow *ui;
    void showEvent(QShowEvent*);
    void showProtectSetting();
    void showActuatorSettings();
    void showLaserSettings();
    void showStoppers();
    void showUsers();
};

#endif // SETTINGSWINDOW_H
