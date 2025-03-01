#include <projectsetting/projectsetting.h>
#include <fstream>
#include <json.hpp>
#include <sstream>
#include <log/log.h>
constexpr std::string_view config_json{"config.json"};

const std::string& ProjectSetting::getProjectName() const
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
    if (_project_name != project_name)
        _project_name = project_name;
    if (_developer_name != developer_name)
        _developer_name = developer_name;
    if (_build_date != build_date)
        _build_date = build_date;
    if (_cmake_args != cmake_args)
        _cmake_args = cmake_args;
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

        std::string cmake_args{};
        for (auto &arg : data["cmakeArgs"])
        {
            if (!cmake_args.empty())
                cmake_args += " ";
            cmake_args += arg;
        };
        set(data["projectName"], data["developerName"], data["build"], cmake_args);
    }
    catch (...)
    {
        Log::log("config.json is not configured correctly!", Type::E_ERROR);
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
    data["projectName"] = _project_name;
    data["developerName"] = _developer_name;
    data["build"] = _build_date;
    std::vector<std::string> args{};
    std::istringstream ss{_cmake_args};
    std::string arg{};
    while (std::getline(ss, arg, ' '))
    {
        args.push_back(arg);
    };
    data["cmakeArgs"] = args;
    out << data;
    out.close();
    return true;
};
