#pragma once
#include <QDialog>

namespace Ui
{
    class AddParamDialog;
}

class HxAddParamDialog : public QDialog
{
    Q_OBJECT

public:
    QStringList names;
    bool isApplyAll = false;
    bool isDefault = false;
    explicit HxAddParamDialog( QWidget* parent = nullptr );
    ~HxAddParamDialog();

private slots:
    void on_btnAdd_clicked();

private:
    Ui::AddParamDialog* ui;
};

