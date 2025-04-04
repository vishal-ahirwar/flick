#include <reproc++/reproc.hpp>
#include <processmanager/processmanager.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>
#include "log/log.h"

// b_log -- should log the msg to the file to print on the termnial ;)

int ProcessManager::startProcess(const std::vector<std::string>&args, std::string &processLog, bool b_log)
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

        std::string_view chunk(reinterpret_cast<char *>(buffer.data()), bytesRead);
        processLog.append(chunk);
        if (b_log)
        {
            Log::log(chunk, Type::E_DISPLAY);
        }
        logFile << chunk;
    }

    int exitCode = 0;
    process.wait(reproc::infinite);
    process.close(reproc::stream::in);
    process.close(reproc::stream::out);
    process.close(reproc::stream::err);
    if (logFile.is_open())
        logFile.close();
    return exitCode;
}
