#ifndef NEWMODELDIALOG_H
#define NEWMODELDIALOG_H

#include <QDialog>
#include "HxModel.h"

namespace Ui
{
    class NewModelDialog;
}

class HxNewModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HxNewModelDialog( QWidget* parent = 0 );
    ~HxNewModelDialog();
    std::shared_ptr<HxModel> data;
private slots:
    void on_btnCreate_clicked();

    void on_txtModelCode_textChanged( const QString& arg1 );

    void on_txtModelName_textChanged( const QString& arg1 );

private:
    Ui::NewModelDialog* ui;
    bool checkInputs();
};

#endif // NEWMODELDIALOG_H
