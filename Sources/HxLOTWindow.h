#pragma once
#include <QMainWindow>
#include <QStandardItem>
#include "HxLOT.h"
#include "HxLOTPropertyDialog.h"

namespace Ui
{
    class LOTWindow;
}

class HxLOTWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxLOTWindow( QWidget* parent = 0 );
    ~HxLOTWindow();

private slots:
    void ControllerPrinted( std::shared_ptr<HxLOT> lot );

    void DataChanged();

    void on_actionNew_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_tbvLots_doubleClicked( const QModelIndex& index );

private:
    Ui::LOTWindow* ui;
    void showEvent( QShowEvent* );
    void ShowLot( QString filter );

};

