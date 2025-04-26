#pragma once
#include "QObject"
#include <QString>
#include "HxUserProfile.h"

class HxProtector : public QObject
{
    Q_OBJECT
private:
    HxUserProfile admin;
    HxUserProfile* _currentUser = nullptr;
public:
    explicit HxProtector( QObject* parent = 0 );
    ~HxProtector();
    bool Login( QString name, QString pass );
    void Logout();
    HxUserProfile* CurrentUser();

signals:
    void LoginChanged();
public slots:

public:
    static HxProtector* Instance();
};

