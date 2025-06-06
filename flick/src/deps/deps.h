#pragma once
#include <projectsetting/projectsetting.h>
#include <string>
#include <string_view>
#include <userinfo/userinfo.h>
#include <vector>

class Deps
{
	ProjectSetting mProjectSetting{};
	UserInfo mUserInfo{};

      public:
	bool buildDeps();
	bool addDeps(const std::string& name, const std::string& version, bool forceUpdateBaseLine = false);
	bool updateCMakeFile(const std::string& vcpkgLog, const std::string& projectName, const std::string& packageName);
	bool installDeps(std::string& vcpkgLog, const std::string_view& TRIPLET);

      private:
	void findCMakeConfig(const std::string& root, std::vector<std::string>& configs);
	bool checkIfLibIsPresentInGlobalDir(const std::string& url);
	bool checkIfLibIsPresentInLocalDir(const std::string& url);
	bool installLocally(const std::string& url);
	bool installGlobally(const std::string& url);
	bool addToConfig(const std::string& path);
	bool rebuildDeps(const std::string& url);
	bool isPackageAvailableOnVCPKG(const std::string& packageName, std::string&, std::string&);
	bool findBuildinBaseline(const std::string& name, const std::string& version, std::string& outBaseLine);
	bool addToJson(const std::string& name, const std::string& version);
	void updateBaseLine(const std::string& baseLine);
};
