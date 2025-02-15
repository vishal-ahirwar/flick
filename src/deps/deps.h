#pragma once
#include <string_view>
#include <string>
#include<projectsetting/projectsetting.h>
#include<vector>
#include<userinfo/userinfo.h>

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
    UserInfo _userinfo{};
public:
    bool buildDeps();
    bool addDeps(const std::string&url);
    DepsSetting& getSetting();
    private:
    bool updateConfig(const std::string&lib_name,const std::string&lib_path);
    bool updateCMakeFile(const std::string&lib_name);
    void findCMakeConfig(const std::string&root,std::vector<std::string>&configs);
    bool checkIfLibIsPresentInGlobalDir(const std::string&url);
    bool checkIfLibIsPresentInLocalDir(const std::string&url);
    bool installLocally(const std::string&url);
    bool installGlobally(const std::string&url);
    bool addToConfig(const std::string&path);
    bool rebuildDeps(const std::string&url);
    
};
