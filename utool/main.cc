#include <iostream>
#include <constants/colors.hpp>
#include <downloader/downloader.h>
#ifdef WIN32
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/aura.exe"};
#else
constexpr std::string_view UPDATE_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/aura"};
#endif

#ifdef WIN32
#define USERNAME "USERPROFILE"
#else
#define USERNAME "USER"
#endif

int main()
{
#ifdef WIN32
    std::string home = getenv(USERNAME);
    home += "\\.aura";
#else
    std::string home{"/home/"};
    home += getenv(USERNAME);
    home+="/.aura";
#endif
    printf("updating aura...\n");
#ifdef WIN32
    Downloader::download(std::string(UPDATE_URL), home + "\\aura.exe");
#else
    Downloader::download(std::string(UPDATE_URL), home + "/aura");
#endif
    printf("%sdone!%s\n", GREEN, WHITE);
    printf("Press any key to quit!\n");
    getchar();

    return 0;
}
