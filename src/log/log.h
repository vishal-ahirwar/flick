#ifndef _LOG_H_
#define _LOG_H_
#include <string_view>
enum class Type
{
    E_DISPLAY,
    E_WARNING,
    E_ERROR,
    E_NONE
};
class Log
{
public:
    static void log(const std::string_view &, Type=Type::E_NONE,const std::string_view&end="\n");
    static void about();
};
#endif