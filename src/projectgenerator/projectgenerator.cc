#include <projectgenerator/projectgenerator.h>
void ProjectGenerator::generate(const Lang &lang)
{
    switch (lang)
    {
    case Lang::C:
    {
        generateCProject();
        break;
    };
    case Lang::CXX:
    {
        generateCXXProject();
        break;
    };
    };
}

void ProjectGenerator::setProjectSetting(const ProjectSetting &project_setting)
{
}

void ProjectGenerator::generateCProject()
{
}

void ProjectGenerator::generateCXXProject()
{
}
