#include <utils/utils.h>
#ifdef _WIN32
#include <windows.h>
#define USERNAME "USERPROFILE"
#else
#include<unistd.h>
#include <cstdlib>
#define USERNAME "USER"
#endif
#include <filesystem>
namespace fs = std::filesystem;
std::string Utils::getFlickPath()
{
    std::string path{std::getenv(USERNAME)};
#ifdef _WIN32
    path += "\\Flick";
#else
    path += "/Flick";
#endif
    if (!fs::exists(path))
        return "";
    return path;
};

int Utils::startApp(const std::string &path)
{
#ifdef _WIN32
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);

    // Create the process
    if (CreateProcessA(path.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        // Wait until the process exits
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Get exit code
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        // Clean up handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return static_cast<int>(exitCode); // Return exit code of the process
    }

    return -1; // Failed to start
#else
    // linux
    pid_t pid = fork(); // Create a new process

    if (pid == -1)
    {
        return -1; // Fork failed
    }
    else if (pid == 0)
    {
        // Child process: Execute the application
        execl(path.c_str(), path.c_str(), (char *)NULL);
        exit(EXIT_FAILURE); // Only reached if execl fails
    }

    return 0; // Parent returns immediately, process runs in the background
#endif
}

