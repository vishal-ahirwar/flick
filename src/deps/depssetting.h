#ifndef DEPS_SETTING_H
#define DEPS_SETTING_H
#include <string>
constexpr std::string_view DEPS_JSON{"external/deps.json"};
class DepsSetting
{
    std::string mCmakeArgs{};

public:
    void set(const std::string &cmakeArgs);
    std::string getCMakeArgs() const;
    bool read();
    void write(const std::string &projectName);
};
#endif