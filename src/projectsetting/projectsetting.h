#ifndef _PROJECT_SETTING_H
#define _PROJECT_SETTING_H
#include<string>
class ProjectSetting
{
    std::string _project_name{};
public:
    const std::string& getProjectName()const;
    void set(const std::string&project_name);
    bool readConfig();
    bool writeConfig(const std::string&path="");
};
#endif