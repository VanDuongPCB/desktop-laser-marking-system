#pragma once
#include "QMainWindow"
#include "QStandardItem"
#include "HxDesign.h"

namespace Ui
{
    class DesignWindow;
}

class HxDesignWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxDesignWindow( QWidget* parent = 0 );
    ~HxDesignWindow();

private:
    Ui::DesignWindow* ui;
    int m_maxBlockCount = 32;
    HxDesignPtr m_pDesign;
    HxDesignPtrMap m_designs;

    void showEvent( QShowEvent* );
    void ShowDesigns();
    void ShowBlocks();
    void ShowParams();


    void OnSelected( const QModelIndex& index );
    void OnSizeChanged( QStandardItem* item );
    void OnBlockChanged( QStandardItem* item );
    void OnInsertParam( const QModelIndex& index );
    void OnRefresh();
    void OnSave();

};

