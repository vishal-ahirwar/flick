#include <deps/deps.h>
#include <json.hpp>
#include <fstream>
#include <log/log.h>
#include "deps.h"
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

void DepsSetting::write()
{
    std::ofstream out{std::string(deps_json)};
    nlohmann::json data;
    data["cmakeArgs"] = nlohmann::json::array({"-DBUILD_SHARED_LIBS=OFF", "-DCMAKE_INSTALL_PREFIX=" + std::string(install_dir)});
    out << data;
    out.close();
};

bool Deps::buildDeps()
{
    _setting.read();
    Log::log(_setting.getCMakeArgs(), Type::E_DISPLAY);
    return false;
}

DepsSetting &Deps::getSetting() 
{
    // TODO: insert return statement here
    return _setting;
};
