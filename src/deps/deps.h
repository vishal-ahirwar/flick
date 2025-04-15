#pragma once
#include <string_view>
#include <string>
#include <projectsetting/projectsetting.h>
#include <vector>
#include <userinfo/userinfo.h>
class Deps
{
    ProjectSetting mProjectSetting{};
    UserInfo mUserInfo{};

public:
    bool buildDeps();
    bool addDeps(const std::string &url);
    bool updateCMakeFile(const std::string &vcpkgLog, const std::string &projectName,const std::string&packageName);
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
    bool isPackageAvailableOnVCPKG(const std::string&packageName);
};
