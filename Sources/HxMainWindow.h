#pragma once
#include <QMainWindow>
#include "HxException.h"

namespace Ui
{
    class MainWindow;
}

class HxMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxMainWindow( QWidget* parent = 0 );
    ~HxMainWindow();
    void SetNavEnable( bool en );
private slots:
    void ErrorReported( HxException ex );
    void LoginChanged();
    void UpdateUI();
    void MenuTabToggled( bool checked );
    void on_actionLogin_triggered();

private:
    Ui::MainWindow* ui;
    std::vector<QAction*> actions;
    std::vector<QWidget*> pages;
    void showEvent( QShowEvent* );
    void closeEvent( QCloseEvent* );
};

