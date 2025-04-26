#ifndef PROTECTOR_H
#define PROTECTOR_H

#include "QObject"
#include <QString>
#include "user.h"

class Protector : public QObject{
    Q_OBJECT
private:
    User admin;
    User *_currentUser = nullptr;
public:
    explicit Protector(QObject *parent = 0);
    ~Protector();
    bool login(QString name, QString pass);
    void logout();
    User *currentUser();

signals:
    void loginChanged();
public slots:

public:
    static Protector *instance();
};

#endif // PROTECTOR_H
