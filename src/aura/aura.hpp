#pragma once

#include <string>
#include<projectsetting/projectsetting.h>
#include<userinfo/userinfo.h>
#include<rt/rt.h>
#include<vector>
#include<constants/constant.hpp>
class Aura
{
private:
    ProjectSetting mProjectSetting{};
    UserInfo mUserInfo{};
    RT mRt{"Aura"};
    std::vector<std::string>mArgs{};
public:
    void createNewProject();
    bool compile();
    void run();
    void build();
    void setup();
    void createInstaller();
    void test();
    void fixInstallation();
    void update();
    void debug();
    bool release();
    void vsCode();
    void reBuild();
    void buildDeps();
    void addDeps();
    void genCMakePreset();
    void createSubProject();
private:
    bool onSetup();
    void setupVcpkg(const std::string&,bool&);
    void installTools(bool&);
    void addToPathUnix();
    void addToPathWin();
    friend void test();
    bool executeCMake(const std::vector<std::string> &additionalCMakeArg);
    const std::string getStandaloneTriplet();
    std::pair<ProjectType, Language> readuserInput();
public:
    Aura(const std::vector<std::string>&args={});
    ~Aura();
};
