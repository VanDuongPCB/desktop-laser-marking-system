#include "HxMainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "HxSettings.h"
#include "HxSettings.h"
#include "HxDesign.h"
#include "HxIVProgram.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxUserProfile.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxLicensing.h"
#include "HxRegisterDialog.h"





int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( "fusion" );

    bool bIsLicensed = HxLicensing::isRegistered();

    if ( !bIsLicensed )
    {
        HxRegisterDialog licenseDialog;
        if ( licenseDialog.exec() )
            bIsLicensed = true;
    }

    if ( !bIsLicensed )
    {
        return 0;
    }

    HxMainWindow w;
    HxSettings::load();
    HxStopper::load();
    HxDesign::load();
    HxIVProgram::load();
    HxModel::load();
    HxLOT::load();
    HxUserProfile::load();

    HxMarker::initialize();

    w.show();
    int code = a.exec();
    HxMarker::terminate();
    return code;
}
