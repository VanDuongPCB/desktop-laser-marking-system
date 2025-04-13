#pragma once
#include "QDialog"

namespace Ui
{
    class RegisterDialog;
}

class HxRegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxRegisterDialog( QWidget* parent = nullptr );
    ~HxRegisterDialog();

private:
    Ui::RegisterDialog* ui;
    void OnRegister();
    void OnMakeKeyOrRegister();
};

