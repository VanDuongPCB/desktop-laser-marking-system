#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "uistyle.h"
#include "settings.h"
#include "design.h"
#include "ivprogram.h"
#include "lot.h"
#include "model.h"
#include "user.h"
#include "stopper.h"
#include "marker.h"
#include "license.h"
#include "registerdialog.h"
#include <iostream>

#include "assert.h"
#include "debugger.h"

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    a.setStyle(new UIStyle());

    bool bIsLicensed = License::isRegistered();

    if(!bIsLicensed)
    {
        RegisterDialog licenseDialog;
        if(licenseDialog.exec())
            bIsLicensed= true;
    }

    if(!bIsLicensed)
    {
        return 0;
    }

    MainWindow w;
    Settings::load();
    Stopper::load();
    Design::load();
    IVProgram::load();
    Model::load();
    Lot::load();
    User::load();

    Marker::initialize();

    w.show();
    int code = a.exec();
    Marker::terminate();
    return code;
}
