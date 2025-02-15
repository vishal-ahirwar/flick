#ifndef PROJECT_GENERATOR
#define PROJECT_GENERATOR
#include <projectsetting/projectsetting.h>

class ProjectGenerator
{
private:
    ProjectSetting _project_setting;

public:
    enum class Lang
    {
        C,
        CXX
    };
    void generate(const Lang &lang);
    void setProjectSetting(const ProjectSetting &project_setting);

private:
    void generateCProject();
    void generateCXXProject();
};
#endif