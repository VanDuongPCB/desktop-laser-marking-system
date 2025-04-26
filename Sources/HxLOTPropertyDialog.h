#pragma once
#include <QDialog>
#include <QStandardItem>
#include "HxLOT.h"
#include "HxModel.h"

namespace Ui
{
    class LOTPropertyDialog;
}

class HxLOTPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxLOTPropertyDialog( QWidget* parent = 0 );
    ~HxLOTPropertyDialog();
    void SetData( std::shared_ptr<HxLOT> data );
    void Clear();
signals:
    void DataChanged();
private slots:
    void ParamChanged( QStandardItem* );

    void on_btnCreateOrChange_clicked();

    void on_txtMACStart_textChanged( const QString& arg1 );

    void on_txtMACEnd_textEdited( const QString& arg1 );

    void on_spxQuantity_valueChanged( int arg1 );

    void on_txtCounterStart_textChanged( const QString& arg1 );

    void on_cbxModel_currentTextChanged( const QString& arg1 );

    void on_txtName_textChanged( const QString& arg1 );

    void on_chxRePrint_toggled( bool checked );

private:
    Ui::LOTPropertyDialog* ui;
    std::shared_ptr<HxLOT> lot;

    void PasteToLot( std::shared_ptr<HxLOT> dstLot, bool newLot );

    void ShowInfo();
    void ShowParams();
    void ShowBlocks();

    bool CheckLotName();
    bool CheckSeriRange();
    bool CheckMacs();
    bool CheckModelInfo();
    bool CheckBlocks();


    void CheckInputs();

};

