#pragma once
#include <QMainWindow>
#include "QLabel"
#include "HxException.h"
#include "HxLOT.h"

#include "HxMarkWindow.h"
#include "HxSettingsWindow.h"
#include "HxTransferWindow.h"
#include "HxControlWindow.h"
#include "HxModelWindow.h"
#include "HxLOTWindow.h"
#include "HxDesignWindow.h"
#include "HxLoginDialog.h"
#include "HxIVProgramWindow.h"
#include "HxLogWindow.h"

namespace Ui
{
    class MainWindow;
}

class HxMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxMainWindow( QWidget* parent = 0 );
    ~HxMainWindow();
private slots:
    void ErrorReported( HxException ex );

private:
    Ui::MainWindow* ui;
    QLabel* m_pLblVersion = nullptr;
    int m_currentTabIndex = 0;

    HxMarkWindow* m_pMarkWindow = nullptr;
    HxTransferWindow* m_pTransferWindow = nullptr;
    HxLOTWindow* m_pLOTWindow = nullptr;
    HxModelWindow* m_pModelWindow = nullptr;
    HxDesignWindow* m_pDesignWindow = nullptr;
    HxIVProgramWindow* m_pIVProgramWindow = nullptr;
    HxSettingsWindow* m_pSettingsWindow = nullptr;
    HxControlWindow* m_pControlWindow = nullptr;
    HxLogWindow* m_pLogWindow = nullptr;


    void resizeEvent( QResizeEvent* event );
    void closeEvent( QCloseEvent* );
    bool eventFilter( QObject* watched, QEvent* event );
    void OnLoginOrLogout();
    void OnLockUI();
    void OnUnLockUIForLeader();
    void OnUnLockUIForSuper();
    void OnUnLockUIForAdmin();
    void OnTabChanged( int index );
};

