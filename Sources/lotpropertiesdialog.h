#ifndef LOTPROPERTIESDIALOG_H
#define LOTPROPERTIESDIALOG_H

#include <QDialog>
#include <QStandardItem>
#include "lot.h"
#include "model.h"

namespace Ui {
class LotPropertiesDialog;
}

class LotPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LotPropertiesDialog(QWidget *parent = 0);
    ~LotPropertiesDialog();
    void setData(std::shared_ptr<Lot> data);
    void clear();
signals:
    void dataChanged();
private slots:
    void paramChanged(QStandardItem *);

    void on_btnCreateOrChange_clicked();

    void on_txtMACStart_textChanged(const QString &arg1);

    void on_txtMACEnd_textEdited(const QString &arg1);

    void on_spxQuantity_valueChanged(int arg1);

    void on_txtCounterStart_textChanged(const QString &arg1);

    void on_cbxModel_currentTextChanged(const QString &arg1);

    void on_txtName_textChanged(const QString &arg1);

    void on_chxRePrint_toggled(bool checked);

private:
    Ui::LotPropertiesDialog *ui;
    std::shared_ptr<Lot> lot;

    void pasteToLot(std::shared_ptr<Lot> dstLot, bool newLot);

    void showInfo();
    void showParams();
    void showBlocks();

    bool checkLotName();
    bool checkSeriRange();
    bool checkMacs();
    bool checkModelInfo();
    bool checkBlocks();


    void checkInputs();

};

#endif // LOTPROPERTIESDIALOG_H
