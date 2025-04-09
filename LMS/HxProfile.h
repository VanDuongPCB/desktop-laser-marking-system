#pragma once
#include <QString>
#include <vector>
#include <memory>

class HxProfile
{
public:
    QString name;
    QString pass;
    bool isAdmin = false;
    bool isSuper = true;
    HxProfile();
    ~HxProfile();

public:
    static std::vector<std::shared_ptr<HxProfile>> items;
    static void load();
    static void save();
};

