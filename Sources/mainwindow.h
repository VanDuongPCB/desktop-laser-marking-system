#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "exception.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setNavEnable(bool en);
private slots:
    void errorReported(Exception ex);
    void loginChanged();
    void updateUI();
    void menuTabToggled(bool checked);
    void on_actionLogin_triggered();

private:
    Ui::MainWindow *ui;
    std::vector<QAction*> actions;
    std::vector<QWidget*> pages;
    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
