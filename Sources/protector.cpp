#include "protector.h"
#include "settings.h"

Protector::Protector(QObject *parent) : QObject(parent){
    admin.name = "Admin";
    admin.pass = Settings::password;
    admin.isAdmin = true;
}

Protector::~Protector(){

}

bool Protector::login(QString name, QString pass){
    name = name.trimmed().toLower();
    if(name == "admin" && pass == Settings::password){
        _currentUser = &admin;
        emit loginChanged();
        return true;
    }

    for(auto &user : User::items){
        if(user->name.trimmed().toLower() == name){
            if(user->pass==""){
                _currentUser = user.get();
                _currentUser->pass = pass;
                User::save();
                emit loginChanged();
            }
            else if(user->pass==pass){
                _currentUser = user.get();
                emit loginChanged();
                return true;
            }
        }
    }
    _currentUser = nullptr;
    emit loginChanged();
    return false;
}

void Protector::logout(){
    _currentUser = nullptr;
    emit loginChanged();
}

User *Protector::currentUser(){
    return _currentUser;
}


/* -------------------------- */
Protector *Protector::instance(){
    static Protector p;
    return &p;
}
