#pragma once
#include <QDialog>
#include <QStandardItem>
#include "HxLOT.h"
#include "HxModel.h"

namespace Ui
{
    class LotPropertiesDialog;
}

class HxLOTPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxLOTPropertyDialog( const QString& lotName, HxLOTPtrMap LOTs, QWidget* parent = 0 );
    ~HxLOTPropertyDialog();

signals:
    void dataChanged();
private slots:
    void OnParamChanged( QStandardItem* );

    // void on_btnCreateOrChange_clicked();

    // void on_txtMACStart_textChanged( const QString& arg1 );

    // void on_txtMACEnd_textEdited( const QString& arg1 );

    // void on_spxQuantity_valueChanged( int arg1 );

    // void on_txtCounterStart_textChanged( const QString& arg1 );

    // void on_cbxModel_currentTextChanged( const QString& arg1 );

    // void on_txtName_textChanged( const QString& arg1 );

private:
    Ui::LotPropertiesDialog* ui;

    bool m_bIsCreate;
    HxLOTPtrMap m_LOTs;
    HxLOTPtr m_pLOT;
    HxModelPtr m_pModel;

    void OnInit(const QString& lotName);


    void showInfo();
    void ShowPrintLo();
    void showParams();
    void showBlocks();

    bool checkLotName();
    bool checkSeriRange();
    bool checkMacs();
    bool checkModelInfo();
    bool checkBlocks();

    void GetInputs();
    bool checkInputs();
    void OnInfoChanged();
    void OnModelChanged();
    void OnApply();
};

