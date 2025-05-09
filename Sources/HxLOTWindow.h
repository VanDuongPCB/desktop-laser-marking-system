#pragma once
#include "QMainWindow"
#include "QStandardItem"
#include "QLineEdit"
#include "HxLOT.h"

namespace Ui
{
    class LotWindow;
}

class HxLOTWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxLOTWindow( QWidget* parent = 0 );
    ~HxLOTWindow();

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
    QLineEdit* m_pSearchTextBox;

    void showEvent( QShowEvent* );
    void showLot( QString filter );

    void OnRefresh();
    void OnFilter( const QString& filter = QString() );
    void OnNew();
    void OnDelete();
    void OnUpdate();
    void OnEdit();
};

