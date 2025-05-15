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
    explicit HxNewModelDialog( HxModelPtrMap models, HxModelPtrMap modelToSaves, QWidget* parent = 0 );
    ~HxNewModelDialog();
    HxModelPtr GetModel() const;
    HxModelPtrMap m_models;
    HxModelPtrMap m_modelToSaves;
    HxModelPtr m_pModel;
private slots:
    void OnCreate();

private:
    Ui::NewModelDialog* ui;
    bool checkInputs();
};

