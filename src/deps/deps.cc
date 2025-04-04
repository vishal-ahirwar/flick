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
    // if (url.find(".git") != std::string::npos)
    //     return system(("cd external && git clone " + url).c_str()) == 0;
    // will use pipe for that
    Log::log(url + " added to vcpkg.json", Type::E_DISPLAY);
    return system(("vcpkg add port " + url).c_str()) == 0;
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
        if(packageName.empty())continue;
        Log::log(packageName, Type::E_DISPLAY);
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
                lines.push_back(package);
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
    args.push_back("vcpkg");
    args.push_back("install");
    args.push_back("--triplet");
    args.push_back(std::string(TRIPLET));
    Log::log("Installing Remaining packages...", Type::E_DISPLAY);
    return ProcessManager::startProcess(args, vcpkgLog, false) == 0;
}
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

bool Deps::checkIfLibIsPresentInGlobalDir(const std::string &url)
{
    return false;
}

bool Deps::checkIfLibIsPresentInLocalDir(const std::string &url)
{
    return false;
}

bool Deps::installLocally(const std::string &url)
{
    return false;
}

bool Deps::installGlobally(const std::string &url)
{
    return false;
}

bool Deps::addToConfig(const std::string &path)
{
    return false;
}

bool Deps::rebuildDeps(const std::string &url)
{
    return false;
}
