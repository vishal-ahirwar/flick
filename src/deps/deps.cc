#include <deps/deps.h>
#include <json.hpp>
#include <fstream>
#include <log/log.h>
#include <filesystem>
#include <sstream>
#include <vector>

constexpr std::string_view CONFIG_CMAKE_ARGS{"-DCMAKE_INSTALL_PREFIX=external/install -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"};
namespace fs = std::filesystem;
void DepsSetting::set(const std::string &cmake_args)
{
    if (_cmake_args != cmake_args)
        _cmake_args = cmake_args;
}
std::string DepsSetting::getCMakeArgs() const

{
    return _cmake_args;
}
bool DepsSetting::read()
{
    std::ifstream in{std::string(deps_json)};
    if (!in.is_open())
    {
        Log::log("deps.json doesn't exist in external aborting!", Type::E_ERROR);
        return false;
    };
    nlohmann::json data;
    in >> data;
    if (data.contains("cmakeArgs"))
        for (auto &args : data["cmakeArgs"])
        {
            _cmake_args += " ";
            _cmake_args += args;
        };
    in.close();
    return true;
}

void DepsSetting::write(const std::string &project_name)
{
    std::ofstream out{project_name + "/" + std::string(deps_json)};
    Log::log("Generating deps.json", Type::E_DISPLAY);
    nlohmann::json data;
    std::istringstream ss{std::string(CONFIG_CMAKE_ARGS)};
    std::string arg{};
    std::vector<std::string> args{};
    while (std::getline(ss, arg, ' '))
    {
        args.push_back(arg);
    };
    data["cmakeArgs"] = args;
    out << data;
    out.close();
};

bool Deps::buildDeps()
{
    _deps_setting.read();
    if (!fs::exists(external_dir))
    {
        Log::log("External directory not found!", Type::E_ERROR);
        return false;
    }

    for (const auto &entry : fs::directory_iterator(external_dir))
    {
        if (entry.is_directory() && entry.path().filename().string() != "install")
        {
            std::string libPath = entry.path().string();
            std::string buildDir = libPath + "/build";
            std::string libName = entry.path().filename().string();

            Log::log("Building: " + libName, Type::E_DISPLAY);

            fs::create_directory(buildDir);
            std::string cmakeCmd = "cmake -S " + libPath + " -B " + buildDir + " -G \"Ninja\" " + _deps_setting.getCMakeArgs();
            std::string buildCmd = "cmake --build " + buildDir + " --target install --parallel";

            if (std::system(cmakeCmd.c_str()) != 0 || std::system(buildCmd.c_str()) != 0)
            {
                Log::log("Failed to build " + libName, Type::E_ERROR);
                continue;
            }
            else
            {
                if (updateConfig(libName))
                    Log::log("adding " + libName + " to config.json", Type::E_DISPLAY);
                if (updateCMakeFile(libName))
                    Log::log("adding " + libName + " to CMakeLists.txt", Type::E_DISPLAY);
            }
        }
    }
    return true;
}

DepsSetting &Deps::getSetting()
{
    // TODO: insert return statement here
    return _deps_setting;
};

bool Deps::updateConfig(const std::string &lib_name)
{
    _project_setting.readConfig();
    std::string lib_config_path{"-D" + lib_name + "_DIR=" + "external/install/lib/cmake/" + lib_name};
    std::istringstream ss{_project_setting.getCMakeArgs()};
    std::string cmake_arg{};
    bool is_in_config{false};
    while (std::getline(ss, cmake_arg, ' '))
    {
        if (cmake_arg.find(lib_config_path) != std::string::npos)
        {
            is_in_config = true;
            break;
        }
    };
    if (!is_in_config)
    {
        _project_setting.set(_project_setting.getProjectName(), _project_setting.getDeveloperName(), _project_setting.getBuildDate(), _project_setting.getCMakeArgs() + " " + lib_config_path);
        _project_setting.writeConfig();
        return true;
    }
    return false;
}

bool Deps::updateCMakeFile(const std::string &lib_name)
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
        if (line.find(lib_name) != std::string::npos)
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
    lines.insert(lines.begin() + pos, "find_package(" + lib_name + " REQUIRED)"); // NOTE
    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines[i].find("@link") != std::string::npos)
        {
            pos = ++i; // add the target_link_libraries(${PROJECT_NAME} after this line
            break;
        }
    };
    lines.insert(lines.begin() + pos, "target_link_libraries(${PROJECT_NAME} " + lib_name + "::" + lib_name + ")"); // NOTE
    std::ofstream out("CMakeLists.txt");
    if (!out.is_open())
        return false;
    for (const auto &l : lines) // writing back to file the updated contents
    {
        out << l << "\n";
    };
    out.close();

    return true;
}
