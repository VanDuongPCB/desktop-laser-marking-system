#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingswindow.h"
#include "markwindow.h"
#include "controlwindow.h"
#include "modelwindow.h"
#include "lotwindow.h"
#include "designwindow.h"
#include "passwindow.h"
#include "logindialog.h"

#include "protector.h"
#include "message.h"

#include "systemerror.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    ui->pageSetting->layout()->addWidget(new SettingsWindow(this));
    ui->pagePass->layout()->addWidget(new PassWindow(this));
    ui->pageMark->layout()->addWidget(new MarkWindow(this));
    ui->pageControl->layout()->addWidget(new ControlWindow(this));
    ui->pageModel->layout()->addWidget(new ModelWindow(this));
    ui->pageScheddule->layout()->addWidget(new LotWindow(this));
    ui->pageDesign->layout()->addWidget(new DesignWindow(this));


    actions.push_back(ui->actionSettings);
    actions.push_back(ui->actionPass);
    actions.push_back(ui->actionMark);
    actions.push_back(ui->actionControl);
    actions.push_back(ui->actionLot);
    actions.push_back(ui->actionModel);
    actions.push_back(ui->actionDesign);
    actions.push_back(ui->actionIVProgram);

    pages.push_back(ui->pageSetting);
    pages.push_back(ui->pagePass);
    pages.push_back(ui->pageMark);
    pages.push_back(ui->pageControl);
    pages.push_back(ui->pageScheddule);
    pages.push_back(ui->pageModel);
    pages.push_back(ui->pageDesign);
    pages.push_back(ui->pageIV);

    for(auto &action : actions){
        connect(action, &QAction::toggled, this, &MainWindow::menuTabToggled);
    }
    ui->actionMark->setChecked(true);

    connect(SystemError::instance(), &SystemError::reported, this, &MainWindow::errorReported);
    connect(Protector::instance(), &Protector::loginChanged, this, &MainWindow::loginChanged);

    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::setNavEnable(bool en){
    ui->toolBar->setEnabled(en);
}

void MainWindow::showEvent(QShowEvent*){

}

void MainWindow::closeEvent(QCloseEvent*){

}

void MainWindow::errorReported(Exception ex){
    Message::error(ex.message, ex.where);
}

void MainWindow::loginChanged(){
    updateUI();
}

void MainWindow::updateUI(){
    User *user = Protector::instance()->currentUser();
    if(user == nullptr){
        ui->actionLogin->setText("Đăng nhập");
        ui->actionSettings->setEnabled(false);
        ui->actionControl->setEnabled(false);
        ui->actionLot->setEnabled(false);
        ui->actionModel->setEnabled(false);
        ui->actionDesign->setEnabled(false);
        ui->actionIVProgram->setEnabled(false);
    }
    else{
        ui->actionLogin->setText("Đăng xuất");
        bool isAdmin = user->isAdmin;
        bool isSuper = user->isSuper;
        bool isWorking = false;
        ui->actionSettings->setEnabled(isAdmin);
        ui->actionControl->setEnabled(isAdmin);
        ui->actionLot->setEnabled(isAdmin || isSuper);
        ui->actionModel->setEnabled(isAdmin);
        ui->actionDesign->setEnabled(isAdmin);
        ui->actionIVProgram->setEnabled(isAdmin);
    }
}

void MainWindow::menuTabToggled(bool ){
    for(int i =0; i < actions.size(); i++){
        QAction *_sender = (QAction*)sender();
        bool match = _sender == actions[i];
        disconnect(actions[i], &QAction::toggled, this, &MainWindow::menuTabToggled);
        if(match){
            ui->stackedWidget->setCurrentWidget(pages[i]);
            actions[i]->setChecked(true);
        }
        else{
            actions[i]->setChecked(false);
        }
        connect(actions[i], &QAction::toggled, this, &MainWindow::menuTabToggled);
    }
}


void MainWindow::on_actionLogin_triggered(){
    if(Protector::instance()->currentUser()!=nullptr){
        int res = Message::warning("Bạn có chắc chắn muốn đăng xuất không ?","Khoan đã");
        if(res == QMessageBox::StandardButton::Yes){
            Protector::instance()->logout();
            ui->actionMark->setChecked(true);
        }
    }
    else{
        LoginDialog dlg(this);
        if(dlg.exec()){

        }
    }
}

