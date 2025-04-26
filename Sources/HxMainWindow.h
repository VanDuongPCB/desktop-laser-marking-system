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
    void setNavEnable( bool en );
private slots:
    void errorReported( HxException ex );
    void loginChanged();
    void updateUI();
    void menuTabToggled( bool checked );
    void on_actionLogin_triggered();

private:
    Ui::MainWindow* ui;
    std::vector<QAction*> actions;
    std::vector<QWidget*> pages;
    void showEvent( QShowEvent* );
    void closeEvent( QCloseEvent* );
};

