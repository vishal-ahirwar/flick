#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <vector>
#include <map>
#include <string>
typedef std::map<std::string, std::vector<std::string>> Packages;
class Extractor
{
    Packages mPackages{};
public:
    const Packages&getPackages()const;
    Packages extract(const std::string&vcpkgLog,std::string);
};
#endif