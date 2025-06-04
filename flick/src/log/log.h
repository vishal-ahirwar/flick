#ifndef _LOG_H_
#define _LOG_H_
#include <string_view>
#include<string>
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
    static void log(const std::string &, Type = Type::E_NONE, const std::string_view &end = "\n");
    static void about();
};

#pragma once
#include <fmt/core.h>
#include <string>

class Logger
{
public:
    enum class Level
    {
        Status,
        Warning,
        Error
    };

    static void status(const std::string &msg,const std::string_view&end="\n")
    {
        print(Level::Status, msg,end);
    }

    static void warning(const std::string &message,const std::string_view&end="\n",
                        const std::string &file = "",
                        int line = -1,
                        int column = -1,
                        const std::string &code = "",
                        const std::string &sourceLine = "",
                        int highlightColumn = -1)
    {
        print(Level::Warning, message, end,file, line, column, code, sourceLine, highlightColumn);
    }

    static void error(const std::string &message,const std::string_view&end="\n",
                      const std::string &file = "",
                      int line = -1,
                      int column = -1,
                      const std::string &code = "",
                      const std::string &sourceLine = "",
                      int highlightColumn = -1)
    {
        print(Level::Error, message, end,file, line, column, code, sourceLine, highlightColumn);
    }

private:
    static void print(Level level,
                      const std::string &message,const std::string_view&end="\n",
                      const std::string &file = "",
                      int line = -1,
                      int column = -1,
                      const std::string &code = "",
                      const std::string &sourceLine = "",
                      int highlightColumn = -1)
    {
        std::string icon, color;
        switch (level)
        {
        case Level::Status:
            icon = "\033[32m✔\033[0m ";
            color = "\033[32m";
            break;
        case Level::Warning:
            icon = "\033[33m⚠ ";
            color = "\033[33m";
            break;
        case Level::Error:
            icon = "\033[31m✖ ";
            color = "\033[31m";
            break;
        }

        // Main message
        fmt::print("{}{} {:<8} {}\033[0m{}", color, icon, toLabel(level), message,end);

        // File + line + column
        if (!file.empty())
        {
            fmt::print("   → File: {}:{}:{}", file, line, column);
            if (!code.empty())
                fmt::print("  [{}]", code);
            fmt::print("\n");
        }

        // Source line highlight
        if (!sourceLine.empty())
        {
            fmt::print("   → Code: {}\n", sourceLine);
            if (highlightColumn >= 0)
            {
                fmt::print("           {}\033[0m\n", std::string(highlightColumn, ' ') + "\033[35m^");
            }
        }

    }

    static std::string toLabel(Level level)
    {
        switch (level)
        {
        case Level::Status:
            return "[flick]";
        case Level::Warning:
            return "[flick]";
        case Level::Error:
            return "[Error]";
        default:
            return "Log";
        }
    }
};

#endif