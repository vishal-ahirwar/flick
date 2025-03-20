#include <log/log.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include <chrono>
#include <auraConfig.h>

void Log::log(const std::string_view &formated_string, Type type, const std::string_view &end)
{
    switch (type)
    {
    case Type::E_DISPLAY:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::white), "{:<8}:  ", "Info");
        fmt::print(fmt::emphasis::faint | fmt::fg(fmt::color::white_smoke), "{}{}", formated_string, end);
        break;
    case Type::E_ERROR:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::red), "{:<8}:  ", "Error");
        fmt::print(fmt::emphasis::faint | fmt::fg(fmt::color::white_smoke), "{}{}", formated_string, end);
        break;
    case Type::E_WARNING:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::yellow), "{:<8}:  ", "Warning");
        fmt::print(fmt::emphasis::faint | fmt::fg(fmt::color::white_smoke), "{}{}", formated_string, end);
        break;
    case Type::E_NONE:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::lawn_green), "{:<8}:  ", "Info");
        fmt::print(fmt::emphasis::faint | fmt::fg(fmt::color::lawn_green), "{}{}", formated_string, end);
        break;
    default:
        break;
    }
    return;
};

void Log::about()
{
    // Title and Description
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::light_sky_blue),
               "About:\n");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Learning C/C++ and tired of creating files and folders and running .cpp,\n"
               "compiling manually again and again?\n"
               "Try this—it does a lot for you!\n"
               "It creates a fully structured project\n"
               "with a CMake file, a clean folder structure, a .gitignore file, and commands to\n"
               "save you time while learning C++ without needing to create\n"
               "a new project every time you want to explore a new concept in C/C++ ;)\n\n");

    // Version Info
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::green),
               "Aura {}.{}.{}\n", AURA_VERSION_MAJOR, AURA_VERSION_MINOR, AURA_VERSION_PATCH);
    fmt::print(fmt::fg(fmt::color::gray),
               "{}\n\n", AURA_COPYRIGHT);

    // Usage Section
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::light_sky_blue),
               "Usage:\n");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "aura <command> projectname\n\n");

    // Available Commands
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::light_sky_blue),
               "Available Commands\n\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  create         - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Create a new C/C++ project. Example: aura create projectName\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  compile        - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Compile the project. Usage: cd into project directory and run 'aura compile'\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  run            - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Run the compiled binary. Ensure you compile the project before running this command.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  help           - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Show the help wizard.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  build          - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Compile and run the project with a single command.\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  builddeps      - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "build all the external libraries present in external directory and install them.\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  adddeps        - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "clone github repo in external directory and build it\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  setup          - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Install a llvm-clang toolchain, ninja and CMake if not already installed.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  utest          - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Enable unit testing. Example: aura create test --test\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  createinstaller- ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Create a packaged build of your application.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  update         - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Update Aura to the latest version.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  rebuild        - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Remove the build directory and recompile the project.\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  ref            - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Only run cmake again to generate cmake files\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  debug          - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Compile in Debug mode and Start the lldb debugger.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  release        - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Compile in release mode.\n");

    fmt::print(fmt::fg(fmt::color::yellow),
               "  vscode         - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Generate VSCode C/C++ configuration files.\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  cmake-preset   - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Generate CMakePresets.json configuration files.\n");
    fmt::print(fmt::fg(fmt::color::yellow),
               "  doctor         - ");
    fmt::print(fmt::fg(fmt::color::white_smoke),
               "Check if All the tools required for C++ Developemnt are installed or install the required tools\n");
}