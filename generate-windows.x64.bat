mkdir IDEs
cd IDEs
mkdir windows.x64.release
cd windows.x64.release
call cmake ../../LMS -DBUILD_TYPE=Release
cd ../..