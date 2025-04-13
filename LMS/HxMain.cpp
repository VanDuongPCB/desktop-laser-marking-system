#include "HxMainWindow.h"
#include "QApplication"
#include "QStyleFactory"
#include "HxUIStyle.h"
#include "HxSettings.h"
#include "HxDesign.h"
#include "HxIVProgram.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxProfile.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxLicense.h"
#include "HxTheme.h"
#include "HxFileManager.h"
#include "HxRegisterDialog.h"
#include "HxDebugger.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new HxUIStyle() );
    GetFileManager()->Init();
    GetTheme()->Refresh();

    if ( !GetLicensing()->isRegistered() )
    {
        HxRegisterDialog licenseDialog;
        licenseDialog.exec();
    }

    if ( !GetLicensing()->isRegistered() )
    {
        return 0;
    }

    HxMainWindow w;
    NxSettings::load();
    HxStopper::load();
    HxDesign::load();
    HxIVProgram::load();
    HxMarker::initialize();
    w.show();
    int code = a.exec();
    HxMarker::terminate();
    return code;
}
