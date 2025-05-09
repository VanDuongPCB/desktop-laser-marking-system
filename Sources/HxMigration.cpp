#include "HxMigration.h"
#include "QApplication"
#include "QString"
#include "QFileInfo"

#include "HxLOT.h"
#include "HxModel.h"
#include "HxDesign.h"
#include "HxProfile.h"
#include "HxStopper.h"
#include "HxSettings.h"



HxMigration::HxMigration()
{
}

HxMigration::~HxMigration()
{
}

void HxMigration::Run()
{
    QString databaseFile = "D:/LMS/LMS.db";
    if ( QFileInfo( databaseFile ).exists() )
        return;

    // show dialog

    // copy
    //QString databaseDefault = qApp->applicationDirPath() + "/Default Files/LMS.db";
    QString databaseDefault = "D:/Workspaces/LMS/Default Files/LMS.db";
    QFile::copy( databaseDefault, databaseFile );

    // migration
    QString oldDataDir = "D:/LMS";
    LOTManager()->Migration( oldDataDir + "/DATA/LOTS" );
    ModelManager()->Migration( oldDataDir + "/DATA/MODELS" );
    DesignManager()->Migration( oldDataDir + "/DATA/DESIGNS" );
    StopperManager()->Migration( oldDataDir + "/SETTINGS" );
    //HxLogManager()->Migration( "" );
}

HxMigration* Migration()
{
    HxMigration instance;
    return &instance;
}