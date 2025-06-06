#ifndef LIBS_H
#define LIBS_H
#include <string>
class Libs
{
	std::string mName{};
	std::string mUrl{};
	std::string mCustomCMakeArgs{};
	bool mBBuildLocal{};

      public:
	std::string getName() const;
	std::string getUrl() const;
	std::string getCustomCMakeArgs() const;
	bool bBuildLocal() const;
	Libs(const std::string& name, const std::string& url, const std::string& customCMakeArgs);
};
#endif