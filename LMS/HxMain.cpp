#include "HxMainWindow.h"
#include <QApplication>
#include <QStyleFactory>
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
#include <HxDebugger.h>

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new HxUIStyle() );
    GetFileManager()->Init();
    GetTheme()->Refresh();


    // HxLOTPtr p = GetLOTManager()->GetLOT("(960K 97880)BOTAN IT PF2L GPF BT RS");
    // HxLOTPtr p = GetLOTManager()->GetLOT("961K-97880_B12222");
    // p->SetQuantity(3);
    // p->SetCounterStart("0000");
    // p->SetValue("FIX1","FFFFF");
    // qDebug () << p->Value("FIX1");

    bool bIsLicensed = GetLicensing()->isRegistered();

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
