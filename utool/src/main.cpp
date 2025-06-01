#include <iostream>
#include <constants/colors.hpp>
#include <downloader/downloader.h>
#include <log/log.h>
#include <rt/rt.h>
#ifdef _WIN32
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/Flick/releases/latest/download/Flick.exe"};
#else
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/Flick/releases/latest/download/Flick"};
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
    home += "\\Flick";
#else
    std::string home{"/home/"};
    home += getenv(USERNAME);
    home += "/Flick";
#endif
    Log::log("Updating Flick...", Type::E_DISPLAY);
#ifdef _WIN32
    Downloader::download(std::string(UPDATE_URL), home + "\\Flick.exe");
#else
    Downloader::download(std::string(UPDATE_URL), home + "/Flick");
#endif
    Log::log("done!", Type::E_DISPLAY);
    Log::log("Press any key to quit!", Type::E_DISPLAY);
    getchar();

    return 0;
}

