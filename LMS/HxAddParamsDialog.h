#pragma once
#include "QDialog"

namespace Ui
{
    class AddParamsDialog;
}

class HxAddParamsDialog : public QDialog
{
    Q_OBJECT

public:
    QStringList m_names;
    bool m_bIsApplyAll = false;
    bool m_bIsDefault = false;
    explicit HxAddParamsDialog( QWidget* parent = nullptr );
    ~HxAddParamsDialog();

private slots:
    void on_btnAdd_clicked();

private:
    Ui::AddParamsDialog* ui;
};

