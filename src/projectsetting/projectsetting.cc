#include <projectsetting/projectsetting.h>
#include "projectsetting.h"

std::string ProjectSetting::getProjectName() const
{
    return _project_name;
}

std::string ProjectSetting::getDeveloperName() const
{
    return _developer_name;
}

std::string ProjectSetting::getBuildDate() const
{
    return _build_date;
}

std::string ProjectSetting::getCMakeArgs() const
{
    return _cmake_args;
}

void ProjectSetting::set(const std::string &project_name, const std::string &developer_name, const std::string &build_date, const std::string &cmake_args)
{
    _project_name = project_name;
    _developer_name = developer_name;
    _build_date = build_date;
    _cmake_args = cmake_args;
};
