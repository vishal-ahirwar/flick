#include <log/log.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include <chrono>
#include <solixconfig.h>

void Log::log(const std::string &formated_string, Type type, const std::string_view &end)
{
    switch (type)
    {
    case Type::E_DISPLAY:
        Logger::status(formated_string);
        break;
    case Type::E_ERROR:
        Logger::error(formated_string);
        break;
    case Type::E_WARNING:
        Logger::warning(formated_string);
        break;
    case Type::E_NONE:
        Logger::status(formated_string);
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
    auto divider = []() {
        fmt::print(fg(color::dim_gray), "────────────────────────────────────────────────────────────\n");
    };

    // Title & Description
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), "📦 About Solix\n\n");
    fmt::print(fg(color::white_smoke),
        "Learning C/C++ and tired of manually creating files, folders, and compiling again and again?\n"
        "Solix automates everything: it sets up a clean project structure with CMake, .gitignore, folders,\n"
        "and helper commands so you can focus on learning and experimenting with C/C++ code.\n\n");

    // Version & Author
    fmt::print(emphasis::bold | fg(color::green), "🌟 Version: ");
    fmt::print(fg(color::white), "{}.{}.{}\n", Solix_VERSION_MAJOR, Solix_VERSION_MINOR, Solix_VERSION_PATCH);
    fmt::print(fg(color::gray), "© 2025 Vishal Ahirwar and all Contributors. All rights reserved.\n\n");

    // Usage
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), "📖 Usage:\n");
    fmt::print(fg(color::white_smoke), "   solix <command> [options] <project-name>\n\n");

    // Commands
    divider();
    fmt::print(emphasis::bold | fg(color::light_sky_blue), "🛠️  Available Commands\n\n");

    auto cmd = [](const std::string& name, const std::string& desc) {
        fmt::print(fg(color::yellow), "   {:<16} ", name);
        fmt::print(fg(color::white_smoke), "- {}\n", desc);
    };

    cmd("create",         "Create a new C/C++ project. e.g. solix create myProject");
    cmd("subproject",     "Create a C/C++ subproject. e.g. solix subproject core");
    cmd("compile",        "Compile the project. Use --standalone for static linking");
    cmd("run",            "Run the compiled binary. Use --args to pass args to executable");
    cmd("build",          "Compile and run. Use --standalone for static linking");
    cmd("install",        "Install external lib via vcpkg to use in your project");
    cmd("setup",          "Install LLVM, Ninja, CMake, and vcpkg if not already installed");
    cmd("tests",          "Enable unit testing. e.g. solix tests");
    cmd("createinstaller","Create a packaged build of your app");
    cmd("update",         "Update Solix to the latest version");
    cmd("rebuild",        "Clean and rebuild. Use --standalone for static linking");
    cmd("debug",          "Compile in Debug mode and start LLDB. Use --standalone");
    cmd("release",        "Build in release mode. Use --standalone for dynamic linking");
    cmd("vscode",         "Generate VSCode C/C++ configuration files");
    cmd("cmake-preset",   "Generate CMakePresets.json");
    cmd("doctor",         "Check and install missing tools for C++ development");

    divider();
}
