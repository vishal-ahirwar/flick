#include <deps/depssetting.h>
#include <fstream>
#include <log/log.h>
#include <json.hpp>
#include <sstream>
#include <map>
constexpr std::string_view CONFIG_CMAKE_ARGS{"-DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"};
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

/*{
    "globalArgs": [
        "-DBUILD_SHARED_LIBS=OFF",
        "-DCMAKE_BUILD_TYPE=Release"
    ],
    "libs": {
        "fmt": {
            "url": "https://github.com/fmtlib/fmt.git",
            "buildLocal": false,
            "cmakeArgs": []
        },
        "json": {
            "url": "https://github.com/nlohmann/json.git",
            "buildLocal": false,
            "cmakeArgs": []
        },
        "openssl": {
            "url": "https://github.com/openssl/openssl.git",
            "buildLocal": false,
            "cmakeArgs": []
        },
        "qt5":{
            "url":"https://github.com/qt/qtbase.git",
            "buildLocal":false,
            "cmakeArgs":[

            ]
        }
    }
}
}*/
// TODO
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
    data["globalArgs"] = args;
    data["libs"]["fmt"] = {
        {"url", "https://github.com/fmtlib/fmt.git"},
        {"localArgs", args},
        {"buildLocal", false}};
    out << data;
    out.close();
};
