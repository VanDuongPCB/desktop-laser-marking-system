#ifndef ADDPARAMSDIALOG_H
#define ADDPARAMSDIALOG_H

#include <QDialog>

namespace Ui {
class AddParamsDialog;
}

class AddParamsDialog : public QDialog
{
    Q_OBJECT

public:
    QStringList names;
    bool isApplyAll = false;
    bool isDefault = false;
    explicit AddParamsDialog(QWidget *parent = nullptr);
    ~AddParamsDialog();

private slots:
    void on_btnAdd_clicked();

private:
    Ui::AddParamsDialog *ui;
};

#endif // ADDPARAMSDIALOG_H
