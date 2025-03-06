#include <projectsetting/projectsetting.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <log/log.h>
constexpr std::string_view config_json{"vcpkg.json"};

const std::string &ProjectSetting::getProjectName() const
{
    return _project_name;
}

void ProjectSetting::set(const std::string &project_name)
{
    if (_project_name != project_name)
        _project_name = project_name;
}
bool ProjectSetting::readConfig()
{
    std::ifstream in{std::string(config_json)};
    if (!in.is_open())
        return false;
    nlohmann::json data{};
    in >> data;
    try
    {
        set(data["name"]);
    }
    catch (...)
    {
        Log::log("vcpkg.json is not configured correctly!", Type::E_ERROR);
        return false;
    }
    in.close();
    return true;
}
bool ProjectSetting::writeConfig(const std::string &path)
{
    std::ofstream out{path + std::string(config_json)};
    if (!out.is_open())
        return false;
    nlohmann::json data{};
    data["name"] = _project_name;
    data["version"] = "1.0.0";
    data["dependencies"]=std::vector<std::string>();
    out << data;
    out.close();
    return true;
};
