#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>

#include "log/log.h"
#include "extractor.h"
#include "deps.h"
#include "processmanager/processmanager.h"

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

DepsSetting &Deps::getSetting()
{
    // TODO: insert return statement here
    return mDepsSetting;
};

bool Deps::addDeps(const std::string &url)
{
    std::string processLog{};
    std::vector<std::string> args{"vcpkg", "add", "port", url};
    return ProcessManager::startProcess(args, processLog,"") == 0;
};

bool Deps::updateCMakeFile(const std::string &vcpkgLog)
{
    std::ifstream in("CMakeLists.txt");
    if (!in.is_open())
    {
        Log::log("Failed to open CMakeLists.txt", Type::E_ERROR);
        return false;
    };
    std::vector<std::string> lines{};
    std::string line{};
    while (std::getline(in, line)) // reading whole file in vector to easily update the file
    {
        lines.push_back(line);
    };
    in.close();
    Extractor extractor;
    if (extractor.extract(vcpkgLog))
    {
        Log::log("Extraction failed!", Type::E_ERROR);
        return false;
    };
    auto packages{extractor.getPackages()};
    Log::log("Packages :", Type::E_DISPLAY);
    for (const auto &[packageName, values] : packages)
    {
        if (packageName.empty())
            continue;
        Log::log("\t+" + packageName, Type::E_DISPLAY);

        for (const auto &package : values)
        {
            bool shouldAdd{true};
            for (const auto &line : lines)
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
                    for (int i = 0; i < lines.size(); ++i)
                    {
                        if (lines.at(i).find("@find") != std::string::npos)
                        {
                            lines.insert(lines.begin() + i + 1, package);
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < lines.size(); ++i)
                    {
                        if (lines.at(i).find("@link") != std::string::npos)
                        {
                            lines.insert(lines.begin() + i + 1, package);
                            break;
                        }
                    }
                }
            }
        }
    };
    std::fstream out{"CMakeLists.txt", std::ios::out};
    if (out.is_open())
    {
        for (const auto &line : lines)
        {
            out << line << "\n";
        };
        out.close();
    }
    return true;
}

bool Deps::installDeps(std::string &vcpkgLog, const std::string_view &TRIPLET)
{
    std::vector<std::string> args{};
    args.push_back("cmake");
    args.push_back("--preset");
    args.push_back(std::string(TRIPLET));
    args.push_back("-Bbuild/debug");
<<<<<<< HEAD
    return ProcessManager::startProcess(args, vcpkgLog, "Installing Remaining packages this may take a while") == 0;
=======
    return ProcessManager::startProcess(args, vcpkgLog, "Installing Remaining packages this may take minutes or even an hour") == 0;
>>>>>>> 744baa22e22d6cee4b5b0978f91f328b8c01c1a9
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
