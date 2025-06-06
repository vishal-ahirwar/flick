#include <log/log.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include <chrono>
#include <flickconfig.h>

void Log::log(const std::string &formated_string, Type type, const std::string_view &end)
{
    switch (type)
    {
    case Type::E_DISPLAY:
        Logger::status(formated_string, end);
        break;
    case Type::E_ERROR:
        Logger::error(formated_string, end);
        break;
    case Type::E_WARNING:
        Logger::warning(formated_string, end);
        break;
    case Type::E_NONE:
        Logger::status(formated_string, end);
        break;
    default:
        break;
    }
    return;
};
void Log::about()
{
    using fmt::color;
    using fmt::emphasis;

    // Divider
    auto divider = []()
    {
        fmt::print(fg(color::dim_gray), "────────────────────────────────────────────────────────────\n");
    };

    // Title & Description
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), R"(
  █████▒██▓     ██▓ ▄████▄   ██ ▄█▀
▓██   ▒▓██▒    ▓██▒▒██▀ ▀█   ██▄█▒
▒████ ░▒██░    ▒██▒▒▓█    ▄ ▓███▄░
░▓█▒  ░▒██░    ░██░▒▓▓▄ ▄██▒▓██ █▄
░▒█░   ░██████▒░██░▒ ▓███▀ ░▒██▒ █▄
 ▒ ░   ░ ▒░▓  ░░▓  ░ ░▒ ▒  ░▒ ▒▒ ▓▒
 ░     ░ ░ ▒  ░ ▒ ░  ░  ▒   ░ ░▒ ▒░
 ░ ░     ░ ░    ▒ ░░        ░ ░░ ░
           ░  ░ ░  ░ ░      ░  ░
                   ░
)");
    fmt::print(fg(color::white_smoke), R"(
flick — Fast. Light. Reliable.

A lightweight CLI tool built for modern C++ developers.  
It handles compilation, dependency setup, and project scaffolding  
with minimal configuration.

Use external libraries in C++ as effortlessly as in Python, Rust, or JavaScript.
flick brings modern dependency management to C++,  
eliminating boilerplate and build headaches.

Built for developers who value speed and simplicity,  
flick finally makes working with C++ feel as clean and modern  
as today’s most developer-friendly languages.

)");

    // Version & Author
    fmt::print(emphasis::bold | fg(color::green), "Version: ");
    fmt::print(fg(color::white), "{}.{}.{}\n", Flick_VERSION_MAJOR, Flick_VERSION_MINOR, Flick_VERSION_PATCH);
    fmt::print(fg(color::gray), "©2025 Vishal Ahirwar and Contributors. All rights reserved.\n\n");

    // Usage
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), "Usage:\n");
    fmt::print(fg(color::white_smoke), "   flick \033[95m<command>\033[0m\n");

    // Commands
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), "Available Commands\n\n");

    auto cmd = [](const std::string &name, const std::string &desc)
    {
        fmt::print(fg(color::light_sky_blue), "   {:<16} ", name);
        fmt::print(fg(color::white_smoke), "- {}\n", desc);
    };

    cmd("new", "Create a new C/C++ project. e.g. \033[95mflick\033[0m new myProject");
    cmd("subproject", "Create a C/C++ subproject. e.g. \033[95mflick\033[0m subproject core");
    cmd("compile", "Compile the project. Use \033[95m--static\033[0m for static linking");
    cmd("run", "Run the compiled binary. Use \033[95m--args\033[0m to pass args to executable");
    cmd("build", "Compile and run. Use \033[95m--static\033[0m for static linking");
    cmd("install", "Install packages to use in your project, \033[95mflick\033[0m install \033[95m--package=fmt --version=11.0.2\033[0m");
    cmd("list", "List all packages used in the current project");
    cmd("init", "Install LLVM, Ninja, CMake, and vcpkg if not already installed");
    cmd("tests", "Enable unit testing. e.g. \033[95mflick\033[0m tests");
    cmd("installer", "Create a packaged build of your app");
    cmd("update", "Update \033[95mflick\033[0m to the latest version");
    cmd("clean", "Clean and rebuild. Use \033[95m--static\033[0m for static linking");
    cmd("debug", "Compile in Debug mode and start LLDB. Use \033[95m--static\033[0m");
    cmd("release", "Build in release mode. Use \033[95m--static\033[0m for dynamic linking");
    cmd("vscode", "Generate VSCode C/C++ configuration files");
    cmd("preset", "Generate CMakePresets.json");
    cmd("doctor", "Check and install missing tools for C++ development");
    cmd("help", "Display this help menu");

    divider();
}
