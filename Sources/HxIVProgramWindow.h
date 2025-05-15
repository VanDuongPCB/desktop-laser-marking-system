#pragma once
#include "QMainWindow"

#include "HxLOT.h"
#include "HxException.h"

namespace Ui
{
    class IVWindow;
}

class HxIVProgramWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HxIVProgramWindow( QWidget* parent = nullptr );
    ~HxIVProgramWindow();

private:
    Ui::IVWindow* ui;
    void OnLoad();
};

