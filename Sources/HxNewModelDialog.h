#pragma once
#include "QDialog"
#include "HxModel.h"

namespace Ui
{
    class NewModelDialog;
}

class HxNewModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxNewModelDialog( HxModelPtrMap models, QWidget* parent = 0 );
    ~HxNewModelDialog();
    HxModelPtr GetModel();
    HxModelPtrMap m_models;
    HxModelPtr m_pModel;
private slots:
    void OnCreate();

private:
    Ui::NewModelDialog* ui;
    bool checkInputs();
};

