#ifndef NEWMODELDIALOG_H
#define NEWMODELDIALOG_H

#include <QDialog>
#include "model.h"

namespace Ui {
class NewModelDialog;
}

class NewModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewModelDialog(QWidget *parent = 0);
    ~NewModelDialog();
    std::shared_ptr<Model> data;
private slots:
    void on_btnCreate_clicked();

    void on_txtModelCode_textChanged(const QString &arg1);

    void on_txtModelName_textChanged(const QString &arg1);

private:
    Ui::NewModelDialog *ui;
    bool checkInputs();
};

#endif // NEWMODELDIALOG_H
