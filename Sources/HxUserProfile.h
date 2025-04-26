#ifndef USER_H
#define USER_H

#include <QString>
#include <vector>
#include <memory>

class HxUserProfile
{
public:
    QString name;
    QString pass;
    bool isAdmin = false;
    bool isSuper = true;
    HxUserProfile();
    ~HxUserProfile();

public:
    static std::vector<std::shared_ptr<HxUserProfile>> items;
    static void load();
    static void save();
};

#endif // USER_H
