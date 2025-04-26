#ifndef REMOVEPARAMDIALOG_H
#define REMOVEPARAMDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveParamDialog;
}

class RemoveParamDialog : public QDialog
{
    Q_OBJECT

public:
    bool isApplyAll = false;
    explicit RemoveParamDialog(QWidget *parent = nullptr);
    ~RemoveParamDialog();
    void setParams(QStringList names);
private slots:
    void on_btnRemove_clicked();

private:
    Ui::RemoveParamDialog *ui;
};

#endif // REMOVEPARAMDIALOG_H
