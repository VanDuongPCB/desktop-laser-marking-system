#ifndef DESIGNWINDOW_H
#define DESIGNWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
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

private slots:
    void blockChanged( QStandardItem* item );

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_tbvDesign_pressed( const QModelIndex& index );

    void on_spxDesignWidth_valueChanged( double arg1 );

    void on_spxDesignHeight_valueChanged( double arg1 );

    void on_tbvModelParams_doubleClicked( const QModelIndex& index );

    void on_tbvLotParams_doubleClicked( const QModelIndex& index );

private:
    Ui::DesignWindow* ui;
    std::vector<std::shared_ptr<HxDesign>> designs;
    std::shared_ptr<HxDesign> design;

    void showEvent( QShowEvent* );

    void showDesigns( QString filter );
    void showSize();
    void showBlocks();
    void showParams();
};

#endif // DESIGNWINDOW_H
