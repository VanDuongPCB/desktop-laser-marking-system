#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>

#include "HxModel.h"

namespace Ui
{
    class ModelWindow;
}

class HxModelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxModelWindow( QWidget* parent = 0 );
    ~HxModelWindow();
private:
    Ui::ModelWindow* ui;
    QLineEdit* txtSearch = nullptr;
    std::shared_ptr<HxModel> selected;
    std::vector<std::shared_ptr<HxModel>> filtereds;

    void showEvent( QShowEvent* );

    void ShowModels();
    void ShowModelInfo();
    void ShowMarkPositions();
    void ShowMarkBlocks();
    void ShowComments();


private slots:
    void ModelFilterTextChanged( QString filter );

    void ModelSelectionChanged( const QModelIndex& index );

    void MarkPositionChanged( QStandardItem* item );

    void CommentChanged( QStandardItem* item );

    void on_actionNew_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_actionFromFile_triggered();

    void on_actionToFile_triggered();



    void on_txtModelNo_textChanged( const QString& arg1 );
    void on_spxProgram_valueChanged( int arg1 );
    void on_spxCvWidth_valueChanged( double arg1 );
    void on_btnAddParam_clicked();
    void on_btnRemoveParam_clicked();
    void on_cbxStopper_currentTextChanged( const QString& arg1 );
    void on_cbxIVPrograms_currentTextChanged( const QString& arg1 );

};

