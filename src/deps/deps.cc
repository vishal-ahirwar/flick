#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>
#include<format>
#include "log/log.h"
#include "extractor.h"
#include "deps.h"
#include "processmanager/processmanager.h"
#include <regex>
namespace fs = std::filesystem;
bool Deps::buildDeps()
{
    // _deps_setting.read();
    // if (!fs::exists(external_dir))
    // {
    //     Log::log("External directory not found!", Type::E_ERROR);
    //     return false;
    // }

    // for (const auto &entry : fs::directory_iterator(external_dir))
    // {
    //     if (entry.is_directory())
    //     {
    //         bool is_cmake_present{false};
    //         for (const auto &cmake_file : fs::directory_iterator(entry.path()))
    //             if (!cmake_file.is_directory() && cmake_file.path().filename().string() == "CMakeLists.txt")
    //             {
    //                 is_cmake_present = true;
    //                 break;
    //             };
    //         if (!is_cmake_present)
    //             continue;
    //         std::string libPath = entry.path().string();
    //         std::string buildDir = libPath + "/build";
    //         std::string libName = entry.path().filename().string();
    //         std::string installDir = libPath + "/install";
    //         Log::log("Building: " + libName, Type::E_DISPLAY);

    //         fs::create_directory(buildDir);
    //         std::string cmakeCmd = "cmake -S " + libPath + " -B " + buildDir + " -G \"Ninja\" " + _deps_setting.getCMakeArgs();
    //         std::string buildCmd = "ninja -C " + buildDir + " -j" + std::to_string(std::thread::hardware_concurrency() - 1);
    //         std::string installCmd = "cmake --install " + buildDir + " --prefix=" + installDir;
    //         if (std::system(cmakeCmd.c_str()) != 0 || std::system(buildCmd.c_str()) != 0 || std::system(installCmd.c_str()) != 0)
    //         {
    //             Log::log("Failed to build " + libName, Type::E_ERROR);
    //             continue;
    //         }
    //         else
    //         {
    //             std::vector<std::string> configs{};
    //             findCMakeConfig(libName, configs);
    //             for (const std::string &config : configs)
    //             {

    //                 if (updateConfig(config, installDir))
    //                     Log::log("adding " + config + " to config.json", Type::E_DISPLAY);
    //                 if (updateCMakeFile(config))
    //                     Log::log("adding " + config + " to CMakeLists.txt", Type::E_DISPLAY);
    //             }
    //         }
    //     }
    // }
    return true;
}

bool Deps::addDeps(const std::string &packageName)
{
    if (!isPackageAvailableOnVCPKG(packageName))
        return false;
    std::string processLog{};
    std::vector<std::string> args{"vcpkg", "add", "port", packageName};
    return ProcessManager::startProcess(args, processLog, "Adding " + packageName + " to vcpkg.json") == 0;
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
    args.push_back("-Bbuild/debug");
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

bool Deps::isPackageAvailableOnVCPKG(const std::string &packageName)
{
    std::vector<std::string> args{"vcpkg", "search", packageName};
    std::string processLog{};
    ProcessManager::startProcess(args, processLog, "Searching Package info");

    std::string line{};
    std::vector<std::string> lines{};
    std::stringstream ss{processLog};
    while (std::getline(ss, line, '\n'))
    {
        if (line.find(packageName) != std::string::npos)
            lines.push_back(line);
    };
    if (lines.size() > 1)
    {
        Log::log("Found more than 1 packages with similiar name " + packageName, Type::E_WARNING);
        for (const auto &package : lines)
            Log::log(package, Type::E_DISPLAY);
    }

    if (lines.size() == 0)
    {
        Log::log("No package found with name " + packageName, Type::E_ERROR);
        return 0;
    }
    std::regex pattern(R"(^(\S+)\s+([^\s]+)\s+(.*)$)");
    for (const auto &package : lines)
    {
        std::smatch matches{};
        if (std::regex_match(package, matches, pattern))
        {
            if (matches[1].str() == packageName)
            {
                Log::log("Selected package info", Type::E_DISPLAY);
                Log::log("\t" + package, Type::E_NONE);
                return true;
            }
            else
            {
                if (lines.size() <= 2)
                    Log::log("You might be looking for : " + package);
            }
        }
    };
    return false;
}
