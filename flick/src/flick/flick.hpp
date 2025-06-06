#pragma once

#include <constants/constant.hpp>
#include <projectsetting/projectsetting.h>
#include <rt/rt.h>
#include <string>
#include <userinfo/userinfo.h>
#include <vector>

class Flick
{
      private:
	ProjectSetting mProjectSetting{};
	UserInfo mUserInfo{};
	RT mRt{"Flick"};
	std::vector<std::string> mArgs{};

      public:
	void createNewProject();
	bool compile();
	void run();
	void build();
	void setup();
	void createInstaller();
	void test();
	void fixInstallation();
	void update();
	void debug();
	bool release();
	void reBuild();
	void buildDeps();
	void addDeps();
	void createSubProject();
	void listPackages();
	void reinit(const std::string& projectDir);

      private:
	void genVSCode(const std::string& projectDir);
	void genCMakePreset();
	void generateClangFiles(const std::string& projectDir);
	void addToPathPermanent(const std::vector<std::string>&);
	bool onSetup();
	void setupVcpkg(const std::string&, bool&);
	void installTools(bool&);
	void addToPathUnix();
	void addToPathWin();
	friend void test();
	bool executeCMake(const std::vector<std::string>& additionalCMakeArg);
	const std::string getStandaloneTriplet();
	std::pair<ProjectType, Language> readuserInput();

      public:
	Flick(const std::vector<std::string>& args = {});
	~Flick();
};
