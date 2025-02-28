#pragma once
#include <string_view>
#include <string>
#include<projectsetting/projectsetting.h>
#include<vector>
#include<userinfo/userinfo.h>
#include<deps/depssetting.h>
constexpr std::string_view external_dir{"external"};
constexpr std::string_view config_json{"config.json"};

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
