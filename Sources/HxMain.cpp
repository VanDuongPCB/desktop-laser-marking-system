#include "HxMainWindow.h"

#include <QApplication>
#include "QLocalSocket"
#include "QLocalServer"
#include <QStyleFactory>

#include "HxSettings.h"
#include "HxSettings.h"
#include "HxDesign.h"
#include "HxIVProgram.h"
#include "HxLOT.h"
#include "HxModel.h"
#include "HxProfile.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxLicense.h"

#include "HxDatabase.h"
#include "HxFileManager.h"
#include "HxMigration.h"
#include "HxRegisterDialog.h"
#include "HxMessage.h"


bool isRunning( const QString& key )
{
    QLocalSocket socket;
    socket.connectToServer( key );
    if ( socket.waitForConnected( 100 ) )
    {
        return true; // Đã có instance khác đang chạy
    }

    QLocalServer::removeServer( key ); // Xóa nếu có server cũ bị kẹt
    return false;
}

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( "fusion" );

    const QString serverName = "LMS-LOCAL-SERVER";
    if ( isRunning( serverName ) )
    {
        HxMsgError( "Phần mềm đang chạy.\nKhông thể mở thêm!" );
        return 0;
    }

    QLocalServer server;
    server.listen( serverName );

    bool bIsLicensed = Licensing()->IsRegistered();

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

    HxDatabase::addDatabase( "QSQLITE", "SQLITE");

    

    //Migration()->Run();

    HxMainWindow w;
    //HxSettings::load();
    //HxStopper::Load();
    //HxDesign::Load();
    //HxIVProgram::Load();
    //HxModel::Load();
    //HxLOT::Load();
    //HxProfile::Load();

    //HxMarker::Initialize();

    w.show();
    int code = a.exec();
    //HxMarker::Terminate();
    return code;
}
