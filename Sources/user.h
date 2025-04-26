#ifndef USER_H
#define USER_H

#include <QString>
#include <vector>
#include <memory>

class User{
public:
    QString name;
    QString pass;
    bool isAdmin = false;
    bool isSuper = true;
    User();
    ~User();

public:
    static std::vector<std::shared_ptr<User>> items;
    static void load();
    static void save();
};

#endif // USER_H
