#pragma once
#include <QMainWindow>
#include "QLabel"
#include "HxException.h"

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

