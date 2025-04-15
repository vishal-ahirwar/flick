#include "extractor.h"
#include <sstream>
#include <log/log.h>
#include <string.h>
#include <regex>
#include <list>
#include <unordered_set>
std::vector<std::pair<std::string, std::regex>> patterns = {
    {"find_package", std::regex(R"(find_package\([^\)]+\))", std::regex_constants::icase)},
    {"target_link_libraries", std::regex(R"(target_link_libraries\([^)]*\))", std::regex_constants::icase)},
    {"find_path", std::regex(R"(find_path\([^\)]+\))", std::regex_constants::icase)},
    {"target_include_directories", std::regex(R"(target_include_directories\([^)]*\))", std::regex_constants::icase)}};

const Packages &Extractor::getPackages() const
{
    return mPackages;
};
int compareWeight(const std::string &a, const std::string &b)
{
    std::string s1 = a;
    std::string s2 = b;

    // Convert both to lowercase
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);

    int weight = 0;
    size_t minLen = std::min(s1.length(), s2.length());

    for (size_t i = 0; i < minLen; ++i)
    {
        if (s1[i] == s2[i])
            ++weight;
        else
            --weight;
    }

    return ((a.length() + b.length())) / weight;
}
std::string getName(const std::string &findPackgeString)
{
    auto start = findPackgeString.find("(");
    auto end = findPackgeString.find(" ");
    if (start == std::string::npos || end == std::string::npos)
        return "";
    return findPackgeString.substr(start + 1, end - start);
}

int Extractor::extract(const std::string &vcpkgLog)
{
    size_t index{0};
    while (true)
    {
        index = vcpkgLog.find("find", index);
        if (index == std::string::npos)
            break;
        auto end = vcpkgLog.find(")", index);
        if (index == std::string::npos)
            break;
        std::string findPackage = vcpkgLog.substr(index, end - index + 1);
        index = end;
        auto name = getName(findPackage);
        index = vcpkgLog.find("target", index);
        if (index == std::string::npos)
            break;
        end = vcpkgLog.find(")", index);
        if (index == std::string::npos)
            break;
        std::string linkTarget = vcpkgLog.substr(index, end - index + 1);
        index = end;
        if (mPackages.contains(name))
            continue;
        mPackages[name] = std::vector{findPackage, linkTarget};
    }
    return 0;
};
