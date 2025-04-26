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
    bool login( QString name, QString pass );
    void logout();
    HxUserProfile* currentUser();

signals:
    void loginChanged();
public slots:

public:
    static HxProtector* instance();
};

