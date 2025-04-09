#pragma once
#include <QDialog>

namespace Ui
{
    class AddUserDialog;
}

class HxAddProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxAddProfileDialog( QWidget* parent = nullptr );
    ~HxAddProfileDialog();

private slots:
    void on_btnAdd_clicked();

    void on_txtUser_textChanged( const QString& arg1 );

private:
    Ui::AddUserDialog* ui;
};
