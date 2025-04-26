#pragma once
#include <QDialog>

namespace Ui
{
    class AddUserDialog;
}

class HxAddUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxAddUserDialog( QWidget* parent = nullptr );
    ~HxAddUserDialog();

private slots:
    void on_btnAdd_clicked();

    void on_txtUser_textChanged( const QString& arg1 );

private:
    Ui::AddUserDialog* ui;
};

