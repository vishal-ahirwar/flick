#include <iostream>
#include <constants/colors.hpp>
#include <downloader/downloader.h>
#include <log/log.h>
#include <rt/rt.h>
#ifdef _WIN32
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/Solix/releases/latest/download/Solix.exe"};
#else
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/Solix/releases/latest/download/Solix"};
#endif

#ifdef _WIN32
#define USERNAME "USERPROFILE"
#else
#define USERNAME "USER"
#endif

int main()
{
    RT rt("utool");
#ifdef _WIN32
    std::string home = getenv(USERNAME);
    home += "\\Solix";
#else
    std::string home{"/home/"};
    home += getenv(USERNAME);
    home += "/Solix";
#endif
    Log::log("updating Solix...", Type::E_DISPLAY);
#ifdef _WIN32
    Downloader::download(std::string(UPDATE_URL), home + "\\Solix.exe");
#else
    Downloader::download(std::string(UPDATE_URL), home + "/Solix");
#endif
    Log::log("done!", Type::E_DISPLAY);
    Log::log("Press any key to quit!", Type::E_DISPLAY);
    getchar();

    return 0;
}
