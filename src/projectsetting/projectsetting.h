#ifndef _PROJECT_SETTING_H
#define _PROJECT_SETTING_H
#include<string>
enum class ProjectState
{
    NONE,
    DEBUG,
    RELEASE,
};
enum class ExternalState
{
    NONE,
    SUCCESS,
    FAILED,
};
enum class ProgrammingLanguage
{
    C,
    CXX
};
class ProjectSetting
{
public:
    ProgrammingLanguage _programming_language = ProgrammingLanguage::CXX;
    ProjectState _project_state = ProjectState::NONE;
    ExternalState _external_state = ExternalState::NONE;
    std::string _project_name{};
};
#endif