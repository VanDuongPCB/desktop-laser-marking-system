#ifndef MARKWINDOW_H
#define MARKWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"

#include "lot.h"

#include "exception.h"

namespace Ui {
class MarkWindow;
}

class MarkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MarkWindow(QWidget *parent = 0);
    ~MarkWindow();

private slots:
    void handleException(Exception ex);
    void markStarted();
    void markStopped();

    void controllerPrinted(std::shared_ptr<Lot> lot);

    void on_actionSelect_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionMark_triggered();

    void on_tbvSelector_doubleClicked(const QModelIndex &index);

    void on_actionLoad_triggered();

private:
    Ui::MarkWindow *ui;
    MainWindow *mainWindow = nullptr;
    std::vector<Exception> exceptions;
    void showEvent(QShowEvent*);
    void showLots();
    void showLotInfo(std::shared_ptr<Lot> lot);
    void showLotStatus(std::shared_ptr<Lot> lot);
    void showLotBlocks();
    void showExceptions();
    void updateUI();
};

#endif // MARKWINDOW_H
