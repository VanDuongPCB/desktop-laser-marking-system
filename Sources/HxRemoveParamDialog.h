#ifndef REMOVEPARAMDIALOG_H
#define REMOVEPARAMDIALOG_H

#include <QDialog>

namespace Ui
{
    class RemoveParamDialog;
}

class HxRemoveParamDialog : public QDialog
{
    Q_OBJECT

public:
    bool isApplyAll = false;
    explicit HxRemoveParamDialog( QWidget* parent = nullptr );
    ~HxRemoveParamDialog();
    void setParams( QStringList names );
private slots:
    void on_btnRemove_clicked();

private:
    Ui::RemoveParamDialog* ui;
};

#endif // REMOVEPARAMDIALOG_H
