#ifndef _CONSTANT_
#define _CONSTANT_
#include <string>
#include <string_view>
static std::string VCPKG_TRIPLET{"default"};

#if defined(_WIN32)
#include <windows.h>
#define USERNAME "USERPROFILE" // Windows environment variable
#elif defined(__linux__)
#include <unistd.h>
#define USERNAME "USER" // Linux environment variable
#elif defined(__APPLE__)
#include <unistd.h>	// For macOS
#define USERNAME "USER" // macOS environment variable
#endif
#ifdef _WIN32
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif

enum class Language { CXX, C, NONE };
enum class ProjectType { NONE, EXECUTABLE, LIBRARY };

#endif
