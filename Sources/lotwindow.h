#ifndef LOTWINDOW_H
#define LOTWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include "lot.h"
#include "lotpropertiesdialog.h"

namespace Ui {
class LotWindow;
}

class LotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LotWindow(QWidget *parent = 0);
    ~LotWindow();

private slots:
    void controllerPrinted(std::shared_ptr<Lot> lot);

    void dataChanged();

    void on_actionNew_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_tbvLots_doubleClicked(const QModelIndex &index);

private:
    Ui::LotWindow *ui;
    void showEvent(QShowEvent *);
    void showLot(QString filter);

};

#endif // LOTWINDOW_H
