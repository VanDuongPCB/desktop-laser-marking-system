#pragma once
#include "QDialog"
#include "QWidget"

#include "HxSettings.h"

namespace Ui
{
    class MigrationDialog;
}


class HxMigrationDialog :public QDialog
{
    Q_OBJECT
public:
    HxMigrationDialog( QWidget* parent = nullptr );
    ~HxMigrationDialog();

private:
    Ui::MigrationDialog* ui;
    HxRegistrySetting m_settings;

    void OnBrowseOldDataDir();
    void OnMigrationLOTs();
    void OnMigrationModels();
    void OnMigrationDesigns();
    void OnMigrationPrintLogs();
    void OnMigrationRePrintLogs();
    void OnMigrationBarcodes();
    void OnMigrationStoppers();
};