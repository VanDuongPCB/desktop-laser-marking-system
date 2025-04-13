#pragma once
#include "QMainWindow"
#include "QStandardItem"
#include "HxLOT.h"
#include "HxLOTPropertyDialog.h"

namespace Ui
{
    class LotWindow;
}

class HxLotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxLotWindow( QWidget* parent = 0 );
    ~HxLotWindow();

private slots:
    void controllerPrinted( std::shared_ptr<HxLOT> lot );

    void dataChanged();

    void on_actionNew_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_tbvLots_doubleClicked( const QModelIndex& index );

private:
    Ui::LotWindow* ui;
    HxLOTPtrMap m_LOTs;


    void showEvent( QShowEvent* );
    void showLot( QString filter );
    void OnRefresh();

};

