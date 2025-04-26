#ifndef PASSWINDOW_H
#define PASSWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"

namespace Ui {
class PassWindow;
}

class PassWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PassWindow(QWidget *parent = 0);
    ~PassWindow();

private slots:
    void passStarted();
    void passStopped();

    void on_btnPass_clicked();

    void on_btnReload_clicked();

    void on_spxCvWidth_valueChanged(double arg1);

    void on_cbxModel_currentTextChanged(const QString &arg1);

    void on_btnStop_clicked();

private:
    Ui::PassWindow *ui;
    MainWindow *mainWindow = nullptr;
};

#endif // PASSWINDOW_H
