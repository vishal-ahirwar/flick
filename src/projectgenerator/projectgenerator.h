#ifndef PROJECT_GENERATOR
#define PROJECT_GENERATOR
#include <projectsetting/projectsetting.h>
#include<userinfo/userinfo.h>
#include <constants/constant.hpp>
class ProjectGenerator
{
private:
    ProjectSetting _project_setting;
    inline static UserInfo _user_info{};

public:
    void generate();
    void setProjectSetting(const ProjectSetting &project_setting, const Language &lang);
    static void readProjectSettings(ProjectSetting *setting);
    static void writeProjectSettings(ProjectSetting *setting);

private:
Language _lang{Language::CXX};

private:
    void generateProject();
    void createDir();
    void generateCppTemplateFile();
    void generateCmakeFile();
    void generateGitIgnoreFile();
    void generateVcpkgFiles();
public:
    static void generateLicenceFile(const UserInfo&user_info);
};
#endif