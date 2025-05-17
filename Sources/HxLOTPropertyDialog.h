#pragma once
#include "QDialog"
#include "QStandardItem"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"

namespace Ui
{
    class LotPropertyDialog;
}

class HxLOTPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxLOTPropertyDialog( HxLOTPtr pLOT, HxLOTPtrMap LOTs, QWidget* parent = 0 );
    ~HxLOTPropertyDialog();
    HxLOTPtr GetLOT() const;

private:
    Ui::LotPropertyDialog* ui;

    bool m_bIsCreate;
    HxLOTPtrMap m_LOTs;
    HxLOTPtr m_pLOT;
    HxModelPtr m_pModel;
    HxDesignPtr m_pDesign;

    void OnInit( HxLOTPtr pLOT );


    void ShowInfo();
    void ShowPrintLo();
    void ShowParams();
    void ShowBlocks();

    bool CheckLotName();
    bool CheckSeriRange();
    bool CheckMacs();
    bool CheckModelInfo();
    bool CheckBlocks();

    void GetInputs();
    bool CheckInputs();
    void OnInfoChanged();
    void OnModelChanged();
    void OnParamChanged( QStandardItem* item );
    void OnApply();
};

