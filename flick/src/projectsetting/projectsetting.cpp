#include <projectsetting/projectsetting.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <log/log.h>
constexpr std::string_view CONFIG_JSON{"vcpkg.json"};

const std::string &ProjectSetting::getProjectName() const
{
    return mProjectName;
}

void ProjectSetting::set(const std::string &projectName)
{
    if (mProjectName != projectName)
        mProjectName = projectName;
}
bool ProjectSetting::readConfig()
{
    std::ifstream in{std::string(CONFIG_JSON)};
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
    std::ofstream out{path + std::string(CONFIG_JSON)};
    if (!out.is_open())
        return false;
    nlohmann::json data{};
    data["name"] = mProjectName;
    data["version"] = "1.0.0";
    data["dependencies"]=std::vector<std::string>();
    out << data.dump(4);
    out.close();
    return true;
};
