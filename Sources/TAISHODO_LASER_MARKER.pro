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

SOURCES += main.cpp\
    devices/actuator.cpp \
    devices/barcodereader.cpp \
    devices/laser.cpp \
    devices/marker.cpp \
    devices/plc.cpp \
    devices/serialport.cpp \
    devices/tcpsocket.cpp \
    fileio/logsaver.cpp \
    license/license.cpp \
    message/message.cpp \
    models/design.cpp \
    models/position.cpp \
    system/exception.cpp \
    system/systemerror.cpp \
    ui/uistyle.cpp \
    windows/addparamsdialog.cpp \
    windows/adduserdialog.cpp \
    windows/mainwindow.cpp \
    settings/settings.cpp \
    fileio/jsonfile.cpp \
    models/model.cpp \
    models/lot.cpp \
    fileio/jsondata.cpp \
    windows/modelwindow.cpp \
    windows/lotwindow.cpp \
    ui/datatable.cpp \
    ui/tableview.cpp \
    models/data.cpp \
    ui/tablewidget.cpp \
    ui/tabwidget.cpp \
    windows/markwindow.cpp \
    windows/designwindow.cpp \
    windows/controlwindow.cpp \
    models/ivprogram.cpp \
    windows/registerdialog.cpp \
    windows/removeparamdialog.cpp \
    windows/settingswindow.cpp \
    windows/lotpropertiesdialog.cpp \
    models/blockinfo.cpp \
    devices/stopper.cpp \
    util/convert.cpp \
    windows/passwindow.cpp \
    windows/newmodeldialog.cpp \
    windows/logindialog.cpp \
    models/user.cpp \
    models/protector.cpp \
    fileio/debugger.cpp \
    fileio/barcodesaver.cpp

HEADERS  += windows/mainwindow.h \
    devices/actuator.h \
    devices/barcodereader.h \
    devices/laser.h \
    devices/marker.h \
    devices/plc.h \
    devices/serialport.h \
    devices/tcpsocket.h \
    fileio/logsaver.h \
    license/license.h \
    message/message.h \
    models/design.h \
    models/position.h \
    settings/settings.h \
    fileio/jsonfile.h \
    models/model.h \
    models/lot.h \
    fileio/jsondata.h \
    system/exception.h \
    system/systemerror.h \
    ui/uistyle.h \
    windows/addparamsdialog.h \
    windows/adduserdialog.h \
    windows/modelwindow.h \
    windows/lotwindow.h \
    ui/datatable.h \
    ui/tableview.h \
    models/data.h \
    ui/tablewidget.h \
    ui/tabwidget.h \
    windows/markwindow.h \
    windows/designwindow.h \
    windows/controlwindow.h \
    models/ivprogram.h \
    windows/registerdialog.h \
    windows/removeparamdialog.h \
    windows/settingswindow.h \
    windows/lotpropertiesdialog.h \
    models/blockinfo.h \
    devices/stopper.h \
    util/convert.h \
    windows/passwindow.h \
    windows/newmodeldialog.h \
    windows/logindialog.h \
    models/user.h \
    models/protector.h \
    fileio/debugger.h \
    fileio/barcodesaver.h

FORMS    += windows/mainwindow.ui \
    windows/addparamsdialog.ui \
    windows/adduserdialog.ui \
    windows/modelwindow.ui \
    windows/lotwindow.ui \
    windows/markwindow.ui \
    windows/designwindow.ui \
    windows/controlwindow.ui \
    windows/registerdialog.ui \
    windows/removeparamdialog.ui \
    windows/settingswindow.ui \
    windows/lotpropertiesdialog.ui \
    windows/passwindow.ui \
    windows/newmodeldialog.ui \
    windows/logindialog.ui

RESOURCES += \
    ui/icons.qrc


