#pragma once
#include "QMainWindow"
#include "QLabel"
#include "QDateEdit"
#include "HxLogger.h"

namespace Ui
{
    class LogWindow;
}

class HxLogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxLogWindow( QWidget* parent = nullptr );
    ~HxLogWindow();

private:
    Ui::LogWindow* ui;
    QDateEdit* m_pDateFrom = nullptr;
    QDateEdit* m_pDateTo = nullptr;
    HxLogArray m_logData;


    void OnSearch();
    void OnExport();
};

