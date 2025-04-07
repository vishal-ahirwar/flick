#include "extractor.h"
#include <sstream>
#include <log/log.h>
#include<string.h>
const Packages &Extractor::getPackages() const
{
    return mPackages;
};

int Extractor::extract(const std::string &vcpkgLog)
{
    std::vector<std::string> values{};
    std::string lastkey{};
    std::string line{};
    std::istringstream logFile(vcpkgLog);
    while (std::getline(logFile, line, '\n'))
    {
        if (line.find("find_package") != std::string::npos || line.find("target_link_libraries") != std::string::npos)
        {
            if (line.find("target_link_libraries") != std::string::npos)
            {
                if (mPackages.contains(lastkey))
                {
                    continue;
                }
                auto startIndex{line.find("main")};
                if (startIndex == std::string::npos)
                {
                    Log::log("Unknown Pattern Found!", Type::E_ERROR);
                    return 1;
                }
                line.replace(startIndex, strlen("main"), "${PROJECT_NAME}");
                startIndex = line.find("target");
                auto endIndex = line.find(")", startIndex);
                values.push_back(line.substr(startIndex, endIndex - startIndex + 1));
                mPackages[lastkey] = values;
                values.clear();
                lastkey = "";
                continue;
            }
            auto startIndex{line.find("(")};
            auto endIndex{line.find(")")};
            if (startIndex == std::string::npos || endIndex == std::string::npos)
            {
                Log::log("Unknown Pattern Found!", Type::E_ERROR);
                return 1;
            };
            std::istringstream ss(line.substr(startIndex + 1, endIndex - startIndex - 1));
            std::string packageName{};
            ss >> packageName;
            if (mPackages.contains(packageName))
            {
                continue;
            }
            lastkey = packageName;
            startIndex = line.find("find");
            endIndex = line.find(")", startIndex);
            values.push_back(line.substr(startIndex, endIndex - startIndex + 1));
        }
    };
    return 0;
};
