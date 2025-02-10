#include <deps/deps.h>
#include <json.hpp>
#include <fstream>
#include <log/log.h>
#include <filesystem>
namespace fs = std::filesystem;
std::string DepsSetting::getCMakeArgs() const
{
    return _cmake_args;
}
void DepsSetting::read()

{
    std::ifstream in{std::string(deps_json)};
    nlohmann::json data;
    in >> data;
    if (data.contains("cmakeArgs"))
        for (auto &args : data["cmakeArgs"])
        {
            _cmake_args += " ";
            _cmake_args += args;
        };
    in.close();
}

void DepsSetting::write(const std::string &project_name)
{
    std::ofstream out{project_name +"/"+std::string(deps_json)};
    nlohmann::json data;
    data["cmakeArgs"] = nlohmann::json::array({"-DBUILD_SHARED_LIBS=OFF", "-DCMAKE_INSTALL_PREFIX=" + std::string(install_dir)});
    out << data;
    out.close();
};

bool Deps::buildDeps()
{
    _setting.read();
    if (!fs::exists(external_dir))
    {
        Log::log("Error: External directory not found!", Type::E_ERROR);
        return false;
    }

    for (const auto &entry : fs::directory_iterator(external_dir))
    {
        if (entry.is_directory()&&entry.path().filename().string()!="install")
        {
            std::string libPath = entry.path().string();
            std::string buildDir = libPath + "/build";
            std::string libName = entry.path().filename().string();

            Log::log("Building: " + libName, Type::E_DISPLAY);

            fs::create_directory(buildDir);
            std::string cmakeCmd = "cmake -S " + libPath + " -B " + buildDir +" -G \"Ninja\" "+_setting.getCMakeArgs();
            std::string buildCmd = "cmake --build " + buildDir + " --target install --parallel";

            if (std::system(cmakeCmd.c_str()) != 0 || std::system(buildCmd.c_str()) != 0)
            {
                Log::log("Failed to build " + libName, Type::E_ERROR);
                continue;
            }
        }
    }
    return true;
}

DepsSetting &Deps::getSetting()
{
    // TODO: insert return statement here
    return _setting;
};
