#pragma once
#include "QString"
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
    static void Load();
    static void Save();
};
