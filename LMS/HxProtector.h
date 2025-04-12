#pragma once
#include <QObject>
#include <QString>
#include "HxProfile.h"

class HxProtector : public QObject
{
    Q_OBJECT
private:
    HxProfilePtr admin;
    HxProfilePtr _currentUser = nullptr;
public:
    explicit HxProtector( QObject* parent = 0 );
    ~HxProtector();
    bool login( QString name, QString pass );
    void logout();
    HxProfilePtr currentUser();

signals:
    void loginChanged();
};


HxProtector* GetProtector();
