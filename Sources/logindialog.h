#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_txtUserName_textChanged(const QString &arg1);

    void on_txtPassword_textChanged(const QString &arg1);

    void on_btnLogin_clicked();

private:
    Ui::LoginDialog *ui;
    bool checkInputs();
};

#endif // LOGINDIALOG_H
