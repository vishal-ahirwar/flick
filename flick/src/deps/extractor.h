#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::vector<std::string>> Packages;
class Extractor
{
	Packages mPackages{};

      public:
	const Packages& getPackages() const;
	Packages extract(const std::string& vcpkgLog, std::string);
};
#endif