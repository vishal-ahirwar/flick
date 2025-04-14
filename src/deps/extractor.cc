#include "extractor.h"
#include <sstream>
#include <log/log.h>
#include <string.h>
#include <regex>
#include <list>
#include <set>
std::vector<std::pair<std::string, std::regex>> patterns = {
    {"find_package", std::regex(R"(find_package\([^\)]+\))")},
    {"target_link_libraries", std::regex(R"(target_link_libraries\([^)]*\))")},
    {"find_path", std::regex(R"(find_path\([^\)]+\))")},
    {"target_include_directories", std::regex(R"(target_include_directories\([^)]*\))")}};

const Packages &Extractor::getPackages() const
{
    return mPackages;
};

int Extractor::extract(const std::string &vcpkgLog)
{
    std::set<std::string> findPackage{};
    std::vector<std::string> targetLink{};

    for (const auto &[name, pattern] : patterns)
    {
        auto start = vcpkgLog.cbegin();
        std::smatch match;
        while (std::regex_search(start, vcpkgLog.cend(), match, pattern))
        {
            if (name.find("find") != std::string::npos)
            {
                findPackage.emplace(match[0]);
            }
            else
            {
                targetLink.push_back(match[0]);
            };
            start = match.suffix().first;
        }
    };
    std::set<std::string> usedTargets;
    for (const auto &package : findPackage)
    {
        std::smatch nameMatch;
        std::string name;
        if (std::regex_search(package, nameMatch, std::regex(R"(find_package\(\s*([^\s\)]+))")))
        {
            name = nameMatch[1];
        }
        else
            continue;

        // Log::log("Found package: " + name);

        std::vector<std::string> values;
        values.push_back(package);

        for (auto &link : targetLink)
        {
            std::smatch targetMatch;
            std::regex targetRegex(name + R"((::[\w\-]+)?)");
            if (std::regex_search(link, targetMatch, targetRegex))
            {
                // Only add the first matching link block
                if (!usedTargets.contains(name))
                {
                    auto index = link.find("main");
                    if (index == std::string::npos)
                    {
                        Log::log("Unkown Pattern!", Type::E_WARNING);
                        continue;
                    }
                    link.replace(index, strlen("main"), "${PROJECT_NAME}");
                    std::string flatLink = std::regex_replace(link, std::regex(R"(\s*\n\s*)"), " ");
                    values.push_back(flatLink);
                    mPackages[name] = values;
                    usedTargets.insert(name);
                }
                break;
            }
        }
    }
    return 0;
};
