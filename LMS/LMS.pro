#-------------------------------------------------
#
# Project created by QtCreator 2023-02-12T09:39:31
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LMS
TEMPLATE = app
RC_ICONS = lms.ico
CONFIG += c++11

# DEFINES += DEBUGGING

RESOURCES += \
    ../Assets/icons.qrc \
    icons.qrc

FORMS += \
    HxAddParamsDialog.ui \
    HxAddProfileDialog.ui \
    HxControlWindow.ui \
    HxDesignWindow.ui \
    HxLOTPropertyDialog.ui \
    HxLOTWindow.ui \
    HxLoginDialog.ui \
    HxMainWindow.ui \
    HxMarkWindow.ui \
    HxModelWindow.ui \
    HxNewModelDialog.ui \
    HxPassWindow.ui \
    HxRegisterDialog.ui \
    HxRemoveParamDialog.ui \
    HxSettingsWindow.ui

HEADERS += \
    HxAddParamsDialog.h \
    HxAddProfileDialog.h \
    HxBarcode.h \
    HxBlockInfo.h \
    HxControlWindow.h \
    HxConvert.h \
    HxDataTable.h \
    HxDebugger.h \
    HxDesign.h \
    HxDesignWindow.h \
    HxException.h \
    HxFileManager.h \
    HxIVProgram.h \
    HxLOT.h \
    HxLOTPropertyDialog.h \
    HxLOTWindow.h \
    HxLaser.h \
    HxLicense.h \
    HxLoginDialog.h \
    HxMainWindow.h \
    HxMarkWindow.h \
    HxMarker.h \
    HxMessage.h \
    HxModel.h \
    HxModelWindow.h \
    HxNewModelDialog.h \
    HxObject.h \
    HxPLC.h \
    HxPassWindow.h \
    HxPosition.h \
    HxProfile.h \
    HxProtector.h \
    HxRegisterDialog.h \
    HxRemoveParamDialog.h \
    HxSerialPort.h \
    HxSettings.h \
    HxSettingsWindow.h \
    HxStopper.h \
    HxSystemError.h \
    HxTabWidget.h \
    HxTableView.h \
    HxTcpSocket.h \
    HxTheme.h \
    HxUIStyle.h

SOURCES += \
    HxAddParamsDialog.cpp \
    HxAddProfileDialog.cpp \
    HxBarcode.cpp \
    HxBlockInfo.cpp \
    HxControlWindow.cpp \
    HxConvert.cpp \
    HxDataTable.cpp \
    HxDebugger.cpp \
    HxDesign.cpp \
    HxDesignWindow.cpp \
    HxException.cpp \
    HxFileManager.cpp \
    HxIVProgram.cpp \
    HxLOT.cpp \
    HxLOTPropertyDialog.cpp \
    HxLOTWindow.cpp \
    HxLaser.cpp \
    HxLicense.cpp \
    HxLoginDialog.cpp \
    HxMain.cpp \
    HxMainWindow.cpp \
    HxMarkWindow.cpp \
    HxMarker.cpp \
    HxMessage.cpp \
    HxModel.cpp \
    HxModelWindow.cpp \
    HxNewModelDialog.cpp \
    HxObject.cpp \
    HxPLC.cpp \
    HxPassWindow.cpp \
    HxPosition.cpp \
    HxProfile.cpp \
    HxProtector.cpp \
    HxRegisterDialog.cpp \
    HxRemoveParamDialog.cpp \
    HxSerialPort.cpp \
    HxSettings.cpp \
    HxSettingsWindow.cpp \
    HxStopper.cpp \
    HxSystemError.cpp \
    HxTabWidget.cpp \
    HxTableView.cpp \
    HxTcpSocket.cpp \
    HxTheme.cpp \
    HxUIStyle.cpp


