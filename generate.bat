@echo off
set APP_VESION=1.0.0
set APP_NAME=LMS

if not defined QT_ROOT_DIR (
    echo "QT_ROOT_DIR is not defined. Please set it to the path of your Qt installation."
    exit /b 1
)

if not defined QT_VERSION (
    echo "QT_VERSION is not defined. Please set it. For example: 4, 5 or 6."
    exit /b 1
)

mkdir _build
cd _build
mkdir %APP_NAME%.%APP_VESION%
set BUILD_DIR=%CD%\\_build\\%APP_NAME%.%APP_VESION%
cd ..

mkdir _ide
cd _ide
mkdir %APP_NAME%
cd %APP_NAME%
cmake ../../LMS
cd ../..