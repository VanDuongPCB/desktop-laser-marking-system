#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QMainWindow>

namespace Ui {
class ControlWindow;
}

class ControlWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlWindow(QWidget *parent = 0);
    ~ControlWindow();

private slots:
    void on_btnBarcodeRead_clicked();

    void on_spxCvWidth_valueChanged(double arg1);

    void on_cbxStopper_currentIndexChanged(int index);

    void on_btnBlockClear_clicked();

    void on_btnMark_clicked();

private:
    Ui::ControlWindow *ui;
};

#endif // CONTROLWINDOW_H
