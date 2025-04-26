#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>

#include "model.h"

namespace Ui {
class ModelWindow;
}

class ModelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModelWindow(QWidget *parent = 0);
    ~ModelWindow();
private:
    Ui::ModelWindow *ui;
    QLineEdit *txtSearch = nullptr;
    std::shared_ptr<Model> selected;
    std::vector<std::shared_ptr<Model>> filtereds;

    void showEvent(QShowEvent*);

    void showModels();
    void showModelInfo();
    void showMarkPositions();
    void showMarkBlocks();
    void showComments();


private slots:
    void modelFilterTextChanged(QString filter);

    void modelSelectionChanged(const QModelIndex &index);

    void markPositionChanged(QStandardItem *item);

    void commentChanged(QStandardItem *item);

    void on_actionNew_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_actionFromFile_triggered();

    void on_actionToFile_triggered();



    void on_txtModelNo_textChanged(const QString &arg1);
    void on_spxProgram_valueChanged(int arg1);
    void on_spxCvWidth_valueChanged(double arg1);
    void on_btnAddParam_clicked();
    void on_btnRemoveParam_clicked();
    void on_cbxStopper_currentTextChanged(const QString &arg1);
    void on_cbxIVPrograms_currentTextChanged(const QString &arg1);

};

#endif // MODELWINDOW_H
