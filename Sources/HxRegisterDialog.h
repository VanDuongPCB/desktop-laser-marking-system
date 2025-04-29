#pragma once
#include <QDialog>

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

private slots:
    void on_pushButton_clicked();

    void on_txtKey_textChanged( const QString& arg1 );

    void on_txtKey_returnPressed();

private:
    Ui::RegisterDialog* ui;
};
