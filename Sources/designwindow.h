#ifndef DESIGNWINDOW_H
#define DESIGNWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include "design.h"

namespace Ui {
class DesignWindow;
}

class DesignWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit DesignWindow(QWidget *parent = 0);
    ~DesignWindow();

private slots:
    void blockChanged(QStandardItem *item);

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_tbvDesign_pressed(const QModelIndex &index);

    void on_spxDesignWidth_valueChanged(double arg1);

    void on_spxDesignHeight_valueChanged(double arg1);

    void on_tbvModelParams_doubleClicked(const QModelIndex &index);

    void on_tbvLotParams_doubleClicked(const QModelIndex &index);

private:
    Ui::DesignWindow *ui;
    std::vector<std::shared_ptr<Design>> designs;
    std::shared_ptr<Design> design;

    void showEvent(QShowEvent*);

    void showDesigns(QString filter);
    void showSize();
    void showBlocks();
    void showParams();
};

#endif // DESIGNWINDOW_H
