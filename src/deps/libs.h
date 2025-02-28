#ifndef LIBS_H
#define LIBS_H
#include <string>
class Libs
{
    std::string _name{};
    std::string _url{};
    std::string _custom_cmake_args{};
    bool _bbuild_local{};
public:
    std::string getName() const;
    std::string getUrl() const;
    std::string getCustomCMakeArgs() const;
    bool bBuildLocal()const;
    Libs(const std::string&name,const std::string&url,const std::string&custom_cmake_args);
};
#endif