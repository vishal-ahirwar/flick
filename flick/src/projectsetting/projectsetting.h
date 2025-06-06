#ifndef _PROJECT_SETTING_H
#define _PROJECT_SETTING_H
#include <string>
class ProjectSetting
{
	std::string mProjectName{};

      public:
	const std::string& getProjectName() const;
	void set(const std::string& projectName);
	bool readConfig();
	bool writeConfig(const std::string& path = "");
};
#endif