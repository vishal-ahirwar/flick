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
public:
    void createNewProject();
    bool compile(const std::string &additional_cmake_arg = "");
    void run(int, const char **);
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
    static void askUserinfo(struct UserInfo *user);
    static void readUserInfoFromConfigFile(UserInfo *user);
    static void writeUserInfoToConfigFile(UserInfo *user);
    static void readProjectSettings(ProjectSetting *setting);
    static void writeProjectSettings(ProjectSetting *setting);

private:

    void readBuildFile();
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

public:
    Aura(const std::vector<std::string>&args={});
    ~Aura();
};
