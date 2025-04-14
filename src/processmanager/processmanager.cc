#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>
#include "log/log.h"
#include "processmanager.h"
int ProcessManager::startProcess(const std::vector<std::string> &args, std::string &processLog, const std::string &msg, bool b_log)
{
    static const char SHAPES[]{'/', '+', '-', '\\'};
    static const int SIZE{sizeof(SHAPES)};

    namespace bp = boost::process;
    namespace fs = std::filesystem;
    namespace ba = boost::algorithm;

    if (!fs::exists("build"))
        fs::create_directories("build");

    std::ofstream logFile("build/build.log", std::ios::out);

    std::string exe = args[0];
    std::vector<std::string> cmdArgs(args.begin() + 1, args.end());

    bp::ipstream outStream;
    bp::ipstream errStream;

    bp::child process(
        bp::search_path(exe),
        bp::args = cmdArgs,
        bp::std_out > outStream,
        bp::std_err > errStream);

    std::string line;
    while (process.running())
    {
        bool hasOutput = false;

        if (std::getline(outStream, line))
        {
            hasOutput = true;
            processLog += line + "\n";
            logFile << line << '\n';

            if (ba::icontains(line, "warning"))
            {
                Log::log(line, Type::E_WARNING);
            }
            else if (ba::icontains(line, "error"))
            {
                Log::log(line, Type::E_ERROR);
            }
        }

        if (std::getline(errStream, line))
        {
            hasOutput = true;
            processLog += line + "\n";
            logFile << line << '\n';

            if (ba::icontains(line, "warning"))
            {
                Log::log(line, Type::E_WARNING);
            }
            else if (ba::icontains(line, "error"))
            {
                Log::log(line, Type::E_ERROR);
            }
        }

        if (hasOutput)
        {
            Log::log(msg + ".." + SHAPES[rand() % SIZE], Type::E_DISPLAY, "\r");
        }
    }

    process.wait();
    int exitCode = process.exit_code();

    if (exitCode != 0)
    {
        Log::log("For more info: " + (fs::current_path() / "build/build.log").generic_string(), Type::E_DISPLAY);
    }

    puts("");
    return exitCode;
}
