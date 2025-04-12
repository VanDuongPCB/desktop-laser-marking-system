#pragma once
#include <QString>
#include <vector>
#include <memory>

class HxProfile;
using HxProfilePtr = std::shared_ptr<HxProfile>;
using HxProfilePtrArray = std::vector<HxProfilePtr>;

class HxProfile
{
public:
    QString name;
    QString pass;
    bool isAdmin = false;
    bool isSuper = true;
    HxProfile();
    ~HxProfile();

// public:
//     static std::vector<std::shared_ptr<HxProfile>> items;
//     static void load();
//     static void save();
};

class HxProfileManager
{
public:
    HxProfilePtrArray GetProfiles();
    HxProfilePtr GetProfile(const QString& name);
    void Save(HxProfilePtr pProfile);
    void Remove(const QString& profileName);
};

HxProfileManager* GetProfileManager();
