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

private:
    Ui::LotWindow* ui;
    HxLOTPtrMap m_LOTs;
    HxLOTPtrMap m_lotToSaves;
    QLineEdit* m_pSearchTextBox;
    
    void showEvent( QShowEvent* );

    void OnRefresh();
    void OnFilter( const QString& filter = QString() );
    void OnNew();
    void OnDelete();
    void OnEdit( const QModelIndex& index );
    void OnSave();
};

