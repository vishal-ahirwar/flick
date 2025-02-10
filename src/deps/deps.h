#pragma once
#include <string_view>
#include <string>
constexpr std::string_view external_dir{"external"};
constexpr std::string_view install_dir{"external/install"};
constexpr std::string_view config_json{"config.json"};
constexpr std::string_view deps_json{"external/deps.json"};
class DepsSetting
{
    std::string _cmake_args{};

public:
    void set(const std::string &cmake_args);
    std::string getCMakeArgs() const;
    void read();
    void write();
};
class Deps
{
    DepsSetting _setting{};

public:
    bool buildDeps();
    DepsSetting& getSetting();
};
