#pragma once

#include <string>
#include<projectsetting/projectsetting.h>
#include<userinfo/userinfo.h>
#include<rt/rt.h>
#include<vector>
class Aura
{
private:
    ProjectSetting _project_setting{};
    UserInfo _user_info{};
    RT _rt{"Aura"};
    std::vector<std::string>_args{};
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
    void refresh();
    void buildDeps();
    void addDeps();
private:
    bool onSetup();
    void setupVcpkg(const std::string&);
    void installEssentialTools(bool &);
    void addToPathUnix();
    void addToPathWin();
    friend void test();
    bool executeCMake(const std::string&);
public:
    Aura(const std::vector<std::string>&args={});
    ~Aura();
};
