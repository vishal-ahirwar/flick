#include <reproc++/reproc.hpp>
#include <processmanager/processmanager.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>
#include "log/log.h"

const char SHAPES[]{'/', '+', '-', '\\'};
const int SIZE{sizeof(SHAPES)};
// b_log -- should log the msg to the file to print on the termnial ;)

int ProcessManager::startProcess(const std::vector<std::string> &args, std::string &processLog, const std::string &msg, bool b_log)
{
    reproc::process process;
    reproc::options options;

    options.redirect.out.type = reproc::redirect::pipe;
    options.redirect.err.type = reproc::redirect::pipe;

    std::error_code ec = process.start(args, options);
    if (ec)
    {
        processLog = "Failed to start process: " + ec.message();
        return 1;
    }

    std::ofstream logFile;
    if (!std::filesystem::exists("build"))
        std::filesystem::create_directories("build");
    logFile.open("build/build.log", std::ios::out);

    std::array<uint8_t, 4096> buffer;

    while (true)
    {
        auto [bytesRead, read_ec] = process.read(reproc::stream::out, buffer.data(), buffer.size());
        if (read_ec || bytesRead == 0)
            break;

        std::string chunk(reinterpret_cast<char *>(buffer.data()), bytesRead);
        processLog.append(chunk);
        Log::log(msg + ".." + SHAPES[rand() % SIZE], Type::E_DISPLAY, "\r");
        if (chunk.find("warning") != std::string::npos)
        {
            std::istringstream ss(chunk);
            std::string line{};
            while (std::getline(ss, line, '\n'))
            {
                if (line.find("warning") != std::string::npos)
                {
                    Log::log(line, Type::E_WARNING);
                }
            }
        }
        else if (chunk.find("error") != std::string::npos)
        {
            std::istringstream ss(chunk);
            std::string line{};
            while (std::getline(ss, line, '\n'))
            {
                if (line.find("error") != std::string::npos)
                {
                    Log::log(line, Type::E_ERROR);
                }
                else
                {
                    if (b_log)
                    {
                        Log::log(chunk, Type::E_DISPLAY);
                    }
                }
            }
        }
    }

    auto [status, exitCode] = process.wait(reproc::infinite);
    process.close(reproc::stream::in);
    process.close(reproc::stream::out);
    process.close(reproc::stream::err);
    logFile << processLog << "\n";
    if (logFile.is_open())
        logFile.close();
    if (status != 0)
    {
        Log::log("For More info " + std::filesystem::path(std::filesystem::current_path().string() + "/build/build.log").generic_string(), Type::E_DISPLAY);
    }
    puts("");
    return status;
}
