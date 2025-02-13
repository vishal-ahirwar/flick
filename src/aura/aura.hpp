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
    static void askUserinfo(struct UserInfo *user);
    static void readUserInfoFromConfigFile(UserInfo *user);
    static void writeUserInfoToConfigFile(UserInfo *user);
    static void readProjectSettings(ProjectSetting *setting);
    static void writeProjectSettings(ProjectSetting *setting);

private:
    void createDir();
    void generateCppTemplateFile();
    void generateCmakeFile();
    void generateGitIgnoreFile();
    void setupUnitTestingFramework();
    void generateLicenceFile();

private:
    bool onSetup();
    void installEssentialTools(bool &);
    void addToPathUnix();
    void addToPathWin();
    friend void test();
    bool executeCMake(const std::string&);
public:
    Aura(const std::vector<std::string>&args={});
    ~Aura();
};
