#include <constants/colors.hpp>
#include <cstdlib>
#include <downloader/downloader.h>
#include <log/log.h>
#include <rt/rt.h>
#include <string>
#include <unzip/unzip.h>
#if defined(_WIN32)
#include <windows.h>
#define USERNAME "USERPROFILE" // Windows environment variable
#define RELEASE_BUILDS_NAME "flick-windows-latest-zip"
#elif defined(__linux__)
#include <unistd.h>
#define USERNAME "USER" // Linux environment variable
#define RELEASE_BUILDS_NAME "flick-ubuntu-latest.zip"
#elif defined(__APPLE__)
#include <unistd.h>	// For macOS
#define USERNAME "USER" // macOS environment variable
#define RELEASE_BUILDS_NAME "flick-macos-13.zip"
#endif

#define BASE_URL "https://github.com/vishal-ahirwar/Flick/releases/latest/download/"
int main()
{
	RT rt("utool");
#ifdef _WIN32
	std::string home = getenv(USERNAME);
	home += "\\flick";
#else
	std::string home{"/home/"};
	home += getenv(USERNAME);
	home += "/flick/";
#endif
	Log::log("Updating Flick...", Type::E_DISPLAY);
	Downloader::download(std::string(BASE_URL) + std::string(RELEASE_BUILDS_NAME), home + RELEASE_BUILDS_NAME);
	Unzip::unzip(home + RELEASE_BUILDS_NAME, home);
	Log::log("done!", Type::E_DISPLAY);
	Log::log("Press any key to quit!", Type::E_DISPLAY);
	getchar();

	return 0;
}
