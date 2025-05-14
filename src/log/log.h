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
    static void log(const std::string_view &, Type = Type::E_NONE, const std::string_view &end = "\n");
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

    static void status(const std::string &msg)
    {
        print(Level::Status, msg);
    }

    static void warning(const std::string &message,
                        const std::string &file = "",
                        int line = -1,
                        int column = -1,
                        const std::string &code = "",
                        const std::string &sourceLine = "",
                        int highlightColumn = -1)
    {
        print(Level::Warning, message, file, line, column, code, sourceLine, highlightColumn);
    }

    static void error(const std::string &message,
                      const std::string &file = "",
                      int line = -1,
                      int column = -1,
                      const std::string &code = "",
                      const std::string &sourceLine = "",
                      int highlightColumn = -1)
    {
        print(Level::Error, message, file, line, column, code, sourceLine, highlightColumn);
    }

private:
    static void print(Level level,
                      const std::string &message,
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
            icon = "✅";
            color = "\033[32m";
            break;
        case Level::Warning:
            icon = "⚠️ ";
            color = "\033[33m";
            break;
        case Level::Error:
            icon = "❌";
            color = "\033[31m";
            break;
        }

        // Print top separator for warning/error
        if (level != Level::Status)
            fmt::print("────────────────────────────────────────────────────────────────────────────\n");

        // Main message
        fmt::print("{}{} {:<8} {}\033[0m\n", color, icon, toLabel(level), message);

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

        if (level != Level::Status)
            fmt::print("────────────────────────────────────────────────────────────────────────────\n");
    }

    static std::string toLabel(Level level)
    {
        switch (level)
        {
        case Level::Status:
            return "Status";
        case Level::Warning:
            return "Warning";
        case Level::Error:
            return "Error";
        default:
            return "Log";
        }
    }
};

#endif