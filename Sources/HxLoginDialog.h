#pragma once
#include <QDialog>

#include "HxProfile.h"

namespace Ui
{
    class LoginDialog;
}

class HxLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxLoginDialog( QWidget* parent = 0 );
    ~HxLoginDialog();

private:
    Ui::LoginDialog* ui;
    void OnLogin();
};

