#pragma once
#include <QDialog>

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

private slots:
    void on_txtUserName_textChanged( const QString& arg1 );

    void on_txtPassword_textChanged( const QString& arg1 );

    void on_btnLogin_clicked();

private:
    Ui::LoginDialog* ui;
    bool CheckInputs();
};

