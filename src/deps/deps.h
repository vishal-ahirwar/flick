#pragma once
#include <string_view>
#include <string>
#include <projectsetting/projectsetting.h>
#include <vector>
#include <userinfo/userinfo.h>
#include <deps/depssetting.h>
constexpr std::string_view EXTERNAL_DIR{"external"};
constexpr std::string_view CONFIG_JSON{"config.json"};

class Deps
{
    DepsSetting mDepsSetting{};
    ProjectSetting mProjectSetting{};
    UserInfo mUserInfo{};

public:
    bool buildDeps();
    bool addDeps(const std::string &url);
    DepsSetting &getSetting();
    bool updateCMakeFile(const std::string &vcpkgLog);
    bool installDeps(std::string&vcpkgLog,const std::string_view&TRIPLET);
private:
    bool updateConfig(const std::string &libName, const std::string &libPath);
    void findCMakeConfig(const std::string &root, std::vector<std::string> &configs);
    bool checkIfLibIsPresentInGlobalDir(const std::string &url);
    bool checkIfLibIsPresentInLocalDir(const std::string &url);
    bool installLocally(const std::string &url);
    bool installGlobally(const std::string &url);
    bool addToConfig(const std::string &path);
    bool rebuildDeps(const std::string &url);
};
