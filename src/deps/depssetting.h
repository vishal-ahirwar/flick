#ifndef DEPS_SETTING_H
#define DEPS_SETTING_H
#include <string>
constexpr std::string_view deps_json{"external/deps.json"};
class DepsSetting
{
    std::string _cmake_args{};

public:
    void set(const std::string &cmake_args);
    std::string getCMakeArgs() const;
    bool read();
    void write(const std::string &project_name);
};
#endif