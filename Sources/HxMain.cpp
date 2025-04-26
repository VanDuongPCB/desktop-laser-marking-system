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

    bool bIsLicensed = HxLicensing::IsRegistered();

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
    HxSettings::Load();
    HxStopper::Load();
    HxDesign::Load();
    HxIVProgram::Load();
    HxModel::Load();
    HxLOT::Load();
    HxUserProfile::Load();

    HxMarker::Initialize();

    w.show();
    int code = a.exec();
    HxMarker::Terminate();
    return code;
}
