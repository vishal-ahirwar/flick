#pragma once

#include <string>
#include<projectsetting/projectsetting.h>
#include<userinfo/userinfo.h>
#include<rt/rt.h>
class Aura
{
private:
    ProjectSetting _project_setting{};
    UserInfo _user_info{};
    RT _rt{"Aura"};
public:
    void createNewProject(const char *argv[], int);
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
    void addConanPackage(const std::string &);
    void reloadPackages();
    void initConan();
    void vsCode();
    void reBuild();
    static void askUserinfo(struct UserInfo *user);
    static void readUserInfoFromConfigFile(UserInfo *user);
    static void writeUserInfoToConfigFile(UserInfo *user);
    static void readProjectSettings(ProjectSetting *setting);
    static void writeProjectSettings(ProjectSetting *setting);
    static bool checkIfConanNeeded(void);

private:
    void generateBuildFile(const std::string &);
    void readBuildFile(std::string &);
    void createDir(const char *);
    void generateCppTemplateFile(const char *);
    void generateCmakeFile(const char *);
    void generateGitIgnoreFile();
    void setupUnitTestingFramework();
    void generateLicenceFile();
    void generateConanFile();

private:
    bool onSetup();
    void installEssentialTools(bool &);
    void addToPathUnix();
    void addToPathWin();
    friend void test();

public:
    Aura();
    ~Aura();
};
