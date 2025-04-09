#pragma once
#include <QObject>
#include <QString>
#include "HxProfile.h"

class HxProtector : public QObject
{
    Q_OBJECT
private:
    HxProfile admin;
    HxProfile* _currentUser = nullptr;
public:
    explicit HxProtector( QObject* parent = 0 );
    ~HxProtector();
    bool login( QString name, QString pass );
    void logout();
    HxProfile* currentUser();

signals:
    void loginChanged();
};


HxProtector* GetProtector();
