#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>
#include "log/log.h"
#include "processmanager.h"
const char SHAPES[]{'/', '+', '-', '\\'};
const int size{sizeof(SHAPES)};
namespace fs = std::filesystem;
int ProcessManager::startProcess(const std::vector<std::string> &args, std::string &processLog, const std::string &msg, bool b_log)
{
    if (!fs::exists("build"))
        fs::create_directories("build");

    std::ofstream logFile("build/build.log", std::ios::out);
    boost::process::ipstream outStream;
    boost::process::ipstream errStream;
    boost::process::child process(boost::process::search_path(args[0]), boost::process::args(std::vector<std::string>(args.begin() + 1, args.end())), boost::process::std_out > outStream, boost::process::std_err > errStream);
    std::string lineOut, lineErr;
    while (process.running() && (std::getline(outStream, lineOut) || std::getline(errStream, lineErr)))
    {
        if (!lineErr.empty())
        {
            if (lineErr.find("warning") != std::string::npos)
                Log::log(lineErr, Type::E_WARNING);
            else if (lineErr.find("error") != std::string::npos)
                Log::log(lineErr, Type::E_ERROR);
            else
                Log::log(lineErr, Type::E_DISPLAY);

            logFile << lineErr << "\n";
            processLog.append(lineErr + "\n");
        }
        if (!lineOut.empty())
        {
            if (lineOut.find("warning") != std::string::npos)
                Log::log(lineOut, Type::E_WARNING);
            else if (lineOut.find("error") != std::string::npos)
                Log::log(lineOut, Type::E_ERROR);
            else
                Log::log(lineOut, Type::E_DISPLAY);
            logFile << lineOut << "\n";
            processLog.append(lineOut + "\n");
        }
    }
    process.wait();
    int exitCode = process.exit_code();
    logFile.close();
    if (exitCode != 0)
    {
        Log::log("For more info: " + (fs::current_path() / "build/build.log").generic_string(), Type::E_DISPLAY);
    }

    puts("");
    return exitCode;
}
