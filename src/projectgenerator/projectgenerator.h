#ifndef PROJECT_GENERATOR
#define PROJECT_GENERATOR
#include <projectsetting/projectsetting.h>
#include<userinfo/userinfo.h>
#include <constants/constant.hpp>
#include<nlohmann/json.hpp>
class ProjectGenerator
{
private:
    ProjectSetting mProjectSetting;
    inline static UserInfo mUserInfo{};
    nlohmann::json mConfig;
public:
    ProjectGenerator();
    void generate();
    void setProjectSetting(const ProjectSetting &projectSetting, const Language &lang);
    static void readProjectSettings(ProjectSetting *setting);
    static void writeProjectSettings(ProjectSetting *setting);
    static void generateCMakePreset(const Language&);
    void generateSubProject(const std::string&subProjectName,bool bIsRoot=false);
    bool getFromConfig(const std::string&key,std::string&result);
private:
Language _lang{Language::CXX};

private:
    void generateProject();
    void createDir();
    void generateCppTemplateFile(bool bIsRoot,const std::string&subProject);
    void generateCmakeFile();
    void generateGitIgnoreFile();
    void generateVcpkgFiles();
public:
    static void generateLicenceFile(const UserInfo&userInfo);
};
#endif