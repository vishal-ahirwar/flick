#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>
#include <format>
#include "log/log.h"
#include "extractor.h"
#include "deps.h"
#include "processmanager/processmanager.h"
#include <regex>
#include <boost/process.hpp>
#include <iostream>
namespace fs = std::filesystem;
bool Deps::buildDeps()
{
    return true;
}

bool Deps::addDeps(const std::string &packageName, const std::string &version, bool forceUpdateBaseLine)
{
    std::string _version{};
    std::string name{};
    if (!isPackageAvailableOnVCPKG(packageName, name, _version))
        ;
    if (!version.empty())
    {
        if (_version != version)
        {
            Log::log(std::format("Package version : {} does not match", version), Type::E_ERROR);
            Log::log("Do you still want to add ?(y/n) ");
            char y{};
            std::cin >> y;
            if (y == 'y')
                _version = version;
            else
                return false;
        }
        else
        {
            _version = version;
        }
    }
    if (name.empty()||_version.empty())
    {
        Log::log("could not find package!", Type::E_ERROR);
        return false;
    }
    std::string processLog{};
    std::ifstream in("vcpkg.json");
    if (!in.is_open())
        return false;
    nlohmann::json data;
    data << in;
    in.close();
    if (data.contains("overrides") && data["overrides"].is_array())
    {
        bool found = false;
        for (auto &overrideEntry : data["overrides"])
        {
            if (overrideEntry.contains("name") && overrideEntry["name"] == name)
            {
                overrideEntry["version"] = _version;
                found = true;
                break;
            }
        }
        if (!found)
        {
            data["overrides"].push_back({{"name", name},
                                         {"version", _version}});
        }
    }
    else
    {
        // Create "overrides" array if it doesn't exist
        data["overrides"] = nlohmann::json::array();
        data["overrides"].push_back({{"name", name},
                                     {"version", _version}});
    }

    if (data.contains("builtin-baseline") == false || forceUpdateBaseLine)
    {
        std::string baseLine{};
        findBuildinBaseline(name, _version, baseLine);
        if (baseLine.empty())
        {
            Log::log(std::format("Built-in baseline not found for version : {}", _version), Type::E_ERROR);
            return false;
        }
        data["builtin-baseline"] = baseLine;
    };
    std::ofstream out("vcpkg.json");
    if (!out.is_open())
        return false;
    out << data.dump(4);
    out.close();
    std::vector<std::string> args{"vcpkg", "add", "port", packageName};
    return ProcessManager::startProcess(args, processLog, "", false) == 0;
};

bool Deps::updateCMakeFile(const std::string &vcpkgLog, const std::string &projectName, const std::string &packageName)
{
    std::fstream subProjectCMake(projectName + "/CMakeLists.txt", std::ios::in);
    std::fstream rootCMake("CMakeLists.txt", std::ios::in);
    if (!rootCMake.is_open())
    {
        Log::log("Failed to open CMakeLists.txt for Reading", Type::E_ERROR);
        return false;
    };
    if (!subProjectCMake.is_open())
    {
        Log::log(std::format("Failed to open {}/CMakeLists.txt for Reading", projectName), Type::E_ERROR);
        return false;
    }
    std::vector<std::string> rootCMakeLines{}, subProjectLines;
    std::string line{};
    while (std::getline(rootCMake, line)) // reading whole file in vector to easily update the file
    {
        rootCMakeLines.push_back(line);
    };
    while (std::getline(subProjectCMake, line))
    {
        subProjectLines.push_back(line);
    };

    rootCMake.close();
    subProjectCMake.close();
    Extractor extractor;
    if (extractor.extract(vcpkgLog))
    {
        Log::log("Extraction failed!", Type::E_ERROR);
        return false;
    };
    auto packages{extractor.getPackages()};
    Log::log("Packages :", Type::E_DISPLAY);
    for (auto &[name, values] : packages)
    {
        if (name.empty())
            continue;
        Log::log("\t+" + name, Type::E_NONE);

        for (auto &package : values)
        {
            bool shouldAdd{true};
            for (const auto &line : rootCMakeLines)
            {
                if (line.find(package) != std::string::npos)
                {
                    shouldAdd = false;
                }
            }
            if (shouldAdd)
            {
                if (package.find("find") != std::string::npos)
                {
                    for (int i = 0; i < rootCMakeLines.size(); ++i)
                    {
                        if (rootCMakeLines.at(i).find("@add_find_package") != std::string::npos)
                        {
                            rootCMakeLines.insert(rootCMakeLines.begin() + i + 1, package);
                            break;
                        }
                    }
                }
                else
                {
                    auto index = package.find("main");
                    if (index == std::string::npos)
                        continue;
                    package.replace(index, strlen("main"), projectName);
                    if (std::find(subProjectLines.begin(), subProjectLines.end(), package) != subProjectLines.end())
                        continue;
                    subProjectLines.push_back(package);
                }
            }
        }
    }
    rootCMake.open("CMakeLists.txt", std::ios::out);
    subProjectCMake.open(projectName + "/CMakeLists.txt", std::ios::out);
    if (!rootCMake.is_open())
    {
        Log::log("Failed to open CMakeLists.txt for Writing", Type::E_ERROR);
        return false;
    };
    if (!subProjectCMake.is_open())
    {
        Log::log(std::format("Failed to open {}/CMakeLists.txt for Writing", projectName), Type::E_ERROR);
        return false;
    }
    for (const auto &line : rootCMakeLines)
        rootCMake << line << "\n";
    for (const auto &line : subProjectLines)
        subProjectCMake << line << "\n";
    rootCMake.close();
    subProjectCMake.close();
    return true;
}

bool Deps::installDeps(std::string &vcpkgLog, const std::string_view &TRIPLET)
{
    std::vector<std::string> args{};
    args.push_back("cmake");
    args.push_back("--preset");
    args.push_back(std::string(TRIPLET));
    return ProcessManager::startProcess(args, vcpkgLog, "Installing Remaining packages this may take a while") == 0;
}
[[deprecated("Will be removed in future")]]
void Deps::findCMakeConfig(const std::string &root, std::vector<std::string> &configs)
{
    if (!fs::exists(root + "/build/install/lib/cmake"))
    {
        configs.push_back(root);
        return;
    }
    for (const auto &lib_path : fs::directory_iterator(root + "/build/install/lib/cmake"))
    {
        if (lib_path.is_directory())
        {
            configs.push_back(lib_path.path().filename().string());
        };
    };
};
[[deprecated("Will be removed in future")]]
bool Deps::checkIfLibIsPresentInGlobalDir(const std::string &url)
{
    return false;
}
[[deprecated("Will be removed in future")]]
bool Deps::checkIfLibIsPresentInLocalDir(const std::string &url)
{
    return false;
}
[[deprecated("Will be removed in future")]]
bool Deps::installLocally(const std::string &url)
{
    return false;
}
[[deprecated("Will be removed in future")]]
bool Deps::installGlobally(const std::string &url)
{
    return false;
}
[[deprecated("Will be removed in future")]]
bool Deps::addToConfig(const std::string &path)
{
    return false;
}
[[deprecated("Will be removed in future")]]
bool Deps::rebuildDeps(const std::string &url)
{
    return false;
}

bool Deps::isPackageAvailableOnVCPKG(const std::string &packageName, std::string &outName, std::string &outVersion)
{
    Log::log("Searching for package : " + packageName, Type::E_DISPLAY);
    std::vector<std::string> args{"search", packageName};
    boost::process::ipstream out;
    boost::process::ipstream err;
    std::vector<std::string>similiarPackages{};
    try
    {
    boost::process::child c(boost::process::search_path("vcpkg"),args,boost::process::std_err > err,boost::process::std_out > out);
    std::string line{};
    std::regex pattern(R"(^(\S+)\s+(\S+(?:#\d+)?|\d{4}-\d{2}-\d{2})\s+(.*))");
    while (std::getline(out, line) || std::getline(err, line))
    {
        std::smatch match{};
        if (std::regex_match(line, match, pattern))
        {
            if (match.size()<3)
            {
                Log::log("Failed to parse vcpkg search output", Type::E_ERROR);
                return false;
            }else if (match[1]!=packageName)
            {
                similiarPackages.push_back(match[1]);
                continue;
            }
            outName = match[1];
            outVersion = match[2];
            Log::log(std::format("Selected Package : {}, Version : {}, About : {}", outName, outVersion, match[3].str()), Type::E_WARNING);
            c.terminate();
            return true;
        }
    }
    }catch (std::exception &e)
    {
        Log::log(std::format("Exception : {}",e.what()),Type::E_ERROR);
        return false;
    }
    if (outName.empty())
    {
        Log::log("Package not found!", Type::E_ERROR);
        Log::log("Did you mean one of these packages ?\n", Type::E_NONE);
        for (const auto&package:similiarPackages)
        {
            Log::log(package,Type::E_NONE);
        }
    }
    return false;
}

bool Deps::findBuildinBaseline(const std::string &name, const std::string &version, std::string &outBaseLine)
{
    std::string vcpkg{std::getenv("VCPKG_ROOT")};
    if (vcpkg.length() <= 0)
    {
        Log::log("VCPKG_ROOT is not Set!", Type::E_ERROR);
        return false;
    }
    boost::process::ipstream out;
    boost::process::ipstream err;
    boost::process::child c(boost::process::search_path("git"), boost::process::args({"-C", vcpkg, "log", "--format=%H %cd %s", "--date=short", "--left-only", "--", std::string("versions/") + name[0] + "-/" + name + ".json"}), boost::process::std_err > err, boost::process::std_out > out);
    std::string line{}, first{};
    std::stringstream ss(name);
    std::getline(ss, first, '-');
    std::string latestBaseLine{};
    while (std::getline(out, line) || std::getline(err, line))
    {
        if (version.length() <= 0)
        {
            if (line.find(first) != std::string::npos)
            {
                auto index = line.find(" ");
                outBaseLine = line.substr(0, index);
                std::string _version = line.substr(index, line.find(" ", index));
                Log::log(std::format("Package : {}, Version : {}, Baseline Commit : {}\n[{}]\n", name, _version, outBaseLine, line), Type::E_NONE);
                c.terminate();
                return true;
            }
        }
        else if (line.find(first) != std::string::npos && line.find(version) != std::string::npos)
        {
            outBaseLine = line.substr(0, line.find(" "));
            Log::log(std::format("Package : {}, Version : {}, Baseline Commit : {}\n[{}]\n", name, version, outBaseLine, line), Type::E_NONE);
            c.terminate();
            return true;
        }else if (latestBaseLine.empty())
        {
            latestBaseLine = line.substr(0, line.find(" "));
        }
    }
    c.wait();
    if (outBaseLine.empty())
    {
        Log::log(std::format("Failed to find Baseline Commit for version : {} using latest baseline commit {}",version,latestBaseLine), Type::E_WARNING);
        outBaseLine=latestBaseLine;
    }
    return true;
}

bool Deps::addToJson(const std::string &name, const std::string &version)
{
    return false;
}

void Deps::updateBaseLine(const std::string &baseLine)
{
}