#include <deps/deps.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <log/log.h>
#include <filesystem>
#include <sstream>
#include <thread>
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
    Log::log(url+" added to vcpkg.json",Type::E_DISPLAY);
    return system(("vcpkg add port " + url).c_str()) == 0;
};

bool Deps::updateCMakeFile(const std::string &libName)
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
        if (line.find(libName) != std::string::npos)
        {
            in.close();
            return false;
        };
        lines.push_back(line);
    };
    in.close();
    auto pos = lines.size() - 1;
    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines[i].find("#@find") != std::string::npos)
        {
            pos = ++i; // add the find_package(name) after this line
            break;
        };
    };
    lines.insert(lines.begin() + pos, "find_package(" + libName + " CONFIG REQUIRED)"); // NOTE
    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines[i].find("@link") != std::string::npos)
        {
            pos = ++i; // add the target_link_libraries(${PROJECT_NAME} after this line
            break;
        }
    };
    lines.insert(lines.begin() + pos, "target_link_libraries(${PROJECT_NAME} " + libName + "::" + libName + ")"); // NOTE
    std::ofstream out("CMakeLists.txt");
    if (!out.is_open())
        return false;
    for (const auto &l : lines) // writing back to file the updated contents
    {
        out << l << "\n";
    };
    out.close();
    Log::log(libName+" added to CMakeLists.txt",Type::E_DISPLAY);
    return true;
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
