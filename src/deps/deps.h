#pragma once
#include <string_view>
#include <string>
#include<projectsetting/projectsetting.h>

constexpr std::string_view external_dir{"external"};
constexpr std::string_view config_json{"config.json"};
constexpr std::string_view deps_json{"external/deps.json"};
class DepsSetting
{
    std::string _cmake_args{};

public:
    void set(const std::string &cmake_args);
    std::string getCMakeArgs() const;
    bool read();
    void write(const std::string&project_name);
};
class Deps
{
    DepsSetting _deps_setting{};
    ProjectSetting _project_setting{};
public:
    bool buildDeps();
    DepsSetting& getSetting();
    private:
    bool updateConfig(const std::string&lib_name);
    bool updateCMakeFile(const std::string&lib_name);
};
