#define NOMINMAX
#define LIBARCHIVE_STATIC
#include <algorithm>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/pipe.hpp>
#include <boost/process/v1/search_path.hpp>
#include <constants/colors.hpp>
#include <constants/constant.hpp>
#include <ctime>
#include <deps/deps.h>
#include <downloader/downloader.h>
#include <filesystem>
#include <flick/flick.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <log/log.h>
#include <processmanager/processmanager.h>
#include <projectgenerator/projectgenerator.h>
#include <regex>
#include <rt/rt.h>
#include <thread>
#include <unittester/unittester.h>
#include <userinfo/userinfo.h>
#include <utils/utils.h>
#include <vector>
#include<unzip/unzip.h>

namespace fs = std::filesystem;

void Flick::addToPathPermanent(const std::vector<std::string>& paths)
{
	std::string bashrc = std::string("/home/") + getenv("USERNAME") + "/.bashrc";
	std::ifstream checkFile(bashrc);
	std::string existingContent;

	if (checkFile.is_open()) {
		existingContent.assign((std::istreambuf_iterator<char>(checkFile)), std::istreambuf_iterator<char>());
		checkFile.close();
	} else {
		Log::log("failed to read ~/.bashrc file!", Type::E_ERROR);
		return;
	}

	std::ofstream bashrcFile(bashrc, std::ios::app);
	if (!bashrcFile.is_open()) {
		Log::log("failed to open ~/.bashrc file for appending!", Type::E_ERROR);
		return;
	}

	for (const auto& path : paths) {
		if (existingContent.find(path) == std::string::npos) {
			bashrcFile << "export PATH=\"$PATH:" << path << "\"\n";
			Log::log(fmt::format("Added to PATH: {}", path.c_str()));
		} else {
			Log::log(fmt::format("Already in PATH: {}", path.c_str()));
		}
	}

	bashrcFile.close();
}

void clearInputBuffer()
{
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.clear();
}

std::pair<ProjectType, Language> Flick::readuserInput()
{
	Log::log("Choose language: \033[95mc / cc \033[0m(default = cc), q = quit > ", Type::E_DISPLAY, "");

	std::string input{};
	std::getline(std::cin, input);
	Language lang{Language::NONE};
	ProjectType projectType{ProjectType::NONE};
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	if (input.empty())
		lang = Language::CXX;
	else if (input == "c")
		lang = Language::C;
	else if (input == "cc")
		lang = Language::CXX;
	else if (input == "q") {
		mRt.~RT();
		std::exit(0);
	}
	Log::log("Choose project type:\033[95m x = executable, l = library \033[0m(Enter to cancel) > ", Type::E_DISPLAY, "");

	// clearInputBuffer();
	std::getline(std::cin, input, '\n');
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	if (input.empty()) {
		mRt.~RT();
		std::exit(0);
	} else if (input == "x") {

		projectType = ProjectType::EXECUTABLE;
	} else if (input == "l") {
		projectType = ProjectType::LIBRARY;
	}

	if (projectType == ProjectType::NONE) {
		Log::log("Unkown Project Type", Type::E_ERROR);
		mRt.~RT();
		std::exit(0);
	}
	return std::pair<ProjectType, Language>{projectType, lang};
};
Flick::Flick(const std::vector<std::string>& args)
{
	mArgs = args;
	std::string cmd{args.at(1)};
	UserInfo::readUserInfoFromConfigFile(&this->mUserInfo);
	if (cmd == "new") {
		if (args.size() < 3) {
			Log::log("Forgot to name your project? Even \033[95m‘untitled’\033[0m would’ve worked", Type::E_ERROR);
			exit(0);
		};
		auto projectName{mArgs.at(2)};
		std::regex pattern("^[a-z][a-z0-9-]*$");
		// if (std::any_of(project_name.begin(), project_name.end(), [](const char &c)
		// 				{ return !((c >= 'a' && c <= 'z') || isdigit(c) || c == '-'); }))
		if (!std::regex_match(projectName, pattern)) {
			Log::log(
			  "Project name must be in lowercase with no space and special characters allowed name : \033[95m^[a-z][a-z0-9-]*$\033[0m",
			  Type::E_ERROR);
			std::exit(0);
		};
		mProjectSetting.set(mArgs.at(2));
		return;
	};
	if (cmd != "init" && cmd != "doctor" && cmd != "update" && cmd != "help")
		ProjectGenerator::readProjectSettings(&this->mProjectSetting);
};
Flick::~Flick() {

};

void Flick::createNewProject()
{
	ProjectGenerator generator{};
	auto info = readuserInput();
	generator.setProjectSetting(mProjectSetting, info.second, info.first);
	generator.generate();
	reinit(mProjectSetting.getProjectName());
};

bool Flick::executeCMake(const std::vector<std::string>& additionalCMakeArg)
{
	std::string processLog{};
	std::vector<std::string> args{"cmake", "-S", ".", "-DENABLE_TESTS=OFF", "-G", "Ninja"};
	for (const auto& cmake : additionalCMakeArg) {
		args.push_back(cmake);
	};
	return ProcessManager::startProcess(args, processLog, "Generating CMake Files this may take a while") == 0;
};

const std::string Flick::getStandaloneTriplet()
{
#if defined(_WIN32)
	return std::string{"windows-static-build"};
#elif defined(__linux__)
	return std::string{"linux-static-build"};
#elif defined(__APPLE__)
	return std::string{"osx-static-build"};
#endif
}
// TODO : add compile option
bool Flick::compile()
{
	// Temp Soln

	namespace fs = std::filesystem;
	std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
	// auto formatedString = fmt::format("Threads in use : {}", cpuThreads.c_str());
	// Log::log(formatedString, Type::E_DISPLAY);
	for (auto& arg : mArgs) {
		if (arg.find("--static") != std::string::npos) {
			VCPKG_TRIPLET = getStandaloneTriplet();
			break;
		}
	};
	if (!fs::exists(fs::current_path().string() + "/build/" + VCPKG_TRIPLET)) {
		// run cmake
		std::vector<std::string> args{"-DCMAKE_BUILD_TYPE=Debug", "--preset=" + std::string(VCPKG_TRIPLET)};
		if (!executeCMake(args)) {
			return false;
		}; // TODO
		// run ninja
		std::string pLog{};
		args.clear();
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/" + VCPKG_TRIPLET);
		args.push_back("-j" + cpuThreads);

		if (ProcessManager::startProcess(args, pLog, "Compiling this may take minutes") ==
		    0) // if there is any kind of error then don't clear the terminal
		{
			Logger::status("BUILD SUCCESSFULL");
			return true;
		} else {
			Logger::error("BUILD FAILED");
			return false;
		}
	} else {
		std::string pLog{};
		std::vector<std::string> args{};
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/" + VCPKG_TRIPLET);
		args.push_back("-j" + cpuThreads);
		// run ninja
		if (ProcessManager::startProcess(args, pLog, "Compiling this may take minutes") ==
		    0) // if there is any kind of error then don't clear the terminal
		{
			Logger::status("BUILD SUCCESSFULL");

			return true;
		} else {
			Logger::error("BUILD FAILED");
			return false;
		}
	}
};
//
void Flick::run()
{
	std::string run{};
	std::string app{mProjectSetting.getProjectName()};
	if (mArgs.size() >= 3) {
		if (mArgs[2].find("--") == std::string::npos) {
			app = mArgs[2];
		};
	}
	for (auto& arg : mArgs) {
		if (arg.find("--static") != std::string::npos) {
			VCPKG_TRIPLET = getStandaloneTriplet();
			break;
		}
	};
#ifdef _WIN32
	run += ".\\build\\" + VCPKG_TRIPLET + "\\";
	run += app;
	run += "\\" + app;
	run += ".exe";
#else
	run += "./build/" + VCPKG_TRIPLET + "/";
	run += app;
	run += "/" + app;
#endif // _WIN32
	if (!fs::exists(run)) {
		run.clear();
#ifdef _WIN32
		run += ".\\build\\" + VCPKG_TRIPLET + "\\";
		run += "\\" + app;
		run += ".exe";
#else
		run += "./build/" + VCPKG_TRIPLET + "/";
		run += "/" + app;
#endif // _WIN32
	}
	if (!fs::exists(run)) {
		Log::log("Subproject \033[95m'" + app + "'\033[0m has no executable. Try verifying the name and rebuilding the project.",
			 Type::E_ERROR);
		return;
	}
	bool isArg{false};
	std::vector<std::string> args{};
	for (auto& arg : mArgs) {
		if (arg.find("--args") != std::string::npos) {
			isArg = true;
			continue;
		}
		if (isArg) {
			args.push_back(arg);
		};
	}
	boost::process::ipstream out;
	boost::process::ipstream err;
	boost::process::child c{run, args};
	// Log::log(fmt::format("Running \033[95m{}\033[0m\n────────────────────────────────────────────────────────────", app), Type::E_DISPLAY);
	c.wait();
	// fmt::println("────────────────────────────────────────────────────────────");
	Log::log(fmt::format("\033[95m{}\033[0m exited with code {}", app, c.exit_code()), Type::E_DISPLAY);
}

//
void Flick::build()
{
	if (!this->compile())
		return;
	this->run();
}
#ifdef _WIN32

bool addToPathPermanentWindows(const std::string& newPath)
{
	HKEY hKey;
	constexpr auto envKey = "Environment";
	constexpr auto pathName = "Path";

	// Open HKEY_CURRENT_USER\Environment
	if (RegOpenKeyExA(HKEY_CURRENT_USER, envKey, 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
		Log::log("Failed to open registry key: HKCU\\Environment", Type::E_ERROR);
		return false;
	}

	// Query existing PATH value
	DWORD type = REG_EXPAND_SZ;
	char buffer[8192];
	DWORD bufferSize = sizeof(buffer);

	std::string currentPath;
	if (RegQueryValueExA(hKey, pathName, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) {
		currentPath.assign(buffer, bufferSize - 1);
	}

	// Check if newPath is already in PATH (case-insensitive on Windows)
	if (_stricmp(currentPath.c_str(), newPath.c_str()) == 0 || currentPath.find(newPath) != std::string::npos) {
		Log::log("Path already exists in PATH.");
		RegCloseKey(hKey);
		return true;
	}

	// Append new path
	std::string updatedPath = currentPath;
	if (!updatedPath.empty() && updatedPath.back() != ';')
		updatedPath += ';';

	updatedPath += newPath;

	// Write new value
	if (RegSetValueExA(hKey, pathName, 0, REG_EXPAND_SZ, reinterpret_cast<const BYTE*>(updatedPath.c_str()),
			   static_cast<DWORD>(updatedPath.size() + 1)) != ERROR_SUCCESS) {
		Log::log("Failed to set PATH in registry.", Type::E_ERROR);
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	// Broadcast WM_SETTINGCHANGE so new shells pick it up
	SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, reinterpret_cast<LPARAM>("Environment"), SMTO_ABORTIFHUNG, 5000, nullptr);

	Log::log(fmt::format("Added to PATH permanently: \033[95m{}\033[0m", newPath));
	return true;
}

//
void Flick::addToPathWin()
{
#ifdef _WIN32
	namespace fs = std::filesystem;

	const char* username = getenv("USERNAME");
	if (!username) {
		Log::log("USERNAME environment variable not found", Type::E_ERROR);
		return;
	}

	std::string flickDir = std::string("C:\\Users\\") + username + "\\flick";
	std::string source = fs::current_path().string() + "\\flick.exe";
	std::string destination = flickDir + "\\flick.exe";

	if (source != destination) {
		try {
			fs::create_directories(flickDir);
		} catch (const std::exception& e) {
			Log::log(fmt::format("Failed to create flick directory: {}", e.what()), Type::E_ERROR);
			return;
		}

		for (const auto& dll : fs::directory_iterator(fs::current_path())) {
			if (dll.is_directory())
				continue;

			if (dll.path().extension() == ".dll") {
				std::string dllDest = flickDir + "\\" + dll.path().filename().string();
				try {
					Log::log(fmt::format("Copying {} to {}", dll.path().filename().string().c_str(), flickDir.c_str()));
					fs::copy_file(dll.path(), dllDest, fs::copy_options::update_existing);
				} catch (const std::exception& e) {
					Log::log(fmt::format("Failed to copy {}: {}", dll.path().filename().string(), e.what()), Type::E_WARNING);
				}
			}
		}

		if (!fs::exists(source)) {
			Log::log("Flick.exe doesn't exist in current directory", Type::E_WARNING);
		} else {
			Log::log(fmt::format("Copying Flick.exe into '{}'", flickDir));
			try {
				fs::copy_file(source, destination, fs::copy_options::update_existing);
				Log::log(fmt::format("{} copied to {}", source, destination));
			} catch (const std::exception& e) {
				Log::log(fmt::format("Error copying Flick.exe: {}", e.what()), Type::E_ERROR);
			}
		}
	}

	// Build desired PATH entries
	std::string desiredPaths = flickDir + ";";

	for (const auto& dir : fs::directory_iterator(flickDir)) {
		if (!dir.is_directory())
			continue;

		const std::string pathStr = dir.path().string();

		if (pathStr.find("_internal") != std::string::npos)
			continue;

		if (pathStr.find("vcpkg") != std::string::npos) {
			desiredPaths += pathStr + ";";
			continue;
		}

		if (pathStr.find("nsis") != std::string::npos) {
			desiredPaths += pathStr + ";" + pathStr + "\\Bin;";
			continue;
		}

		desiredPaths += pathStr + "\\bin;";
	}

	std::string currentEnvPath = getenv("PATH") ? getenv("PATH") : "";
	std::istringstream pathStream(desiredPaths);
	std::string singlePath;
	std::string missingPaths;
	bool allFound = true;

	while (std::getline(pathStream, singlePath, ';')) {
		if (singlePath.empty())
			continue;

		if (currentEnvPath.find(singlePath) == std::string::npos) {
			allFound = false;
			missingPaths += singlePath + ";";
		}
	}

	if (allFound) {
		Log::log("All Flick paths are already in PATH", Type::E_DISPLAY);
	} else {
		Log::log("Some Flick paths are missing from PATH. Adding them permanently. Restart your shell or system to apply changes.",
			 Type::E_WARNING);

		std::istringstream missingStream(missingPaths);
		std::string pathPart;
		while (std::getline(missingStream, pathPart, ';')) {
			if (!pathPart.empty()) {
				Log::log(pathPart);
				addToPathPermanentWindows(pathPart); // your implementation
			}
		}
	}
#endif
}
#endif

//
void Flick::addToPathUnix()
{
	namespace fs = std::filesystem;
	std::string homeDir = std::string("/home/") + getenv(USERNAME);
	std::string flickDir = homeDir + "/flick";
	std::string source = fs::current_path().string() + "/flick";
	std::string destination = flickDir + "/flick";

	if (source != destination) {
		if (!fs::exists(source)) {
			Log::log("Flick doesn't exist in current dir", Type::E_ERROR);
			return;
		}

		Log::log(fmt::format("Copying Flick into {}", destination.c_str()));
		fs::remove(destination);
		if (fs::copy_file(source, destination, fs::copy_options::overwrite_existing)) {
			Log::log(fmt::format("{} copied to {}", source.c_str(), destination.c_str()));
		} else {
			Log::log("Error while copying Flick into flick directory!", Type::E_ERROR);
			return;
		}
	}

	std::vector<std::string> flickPaths;
	flickPaths.push_back(flickDir); // main flick path
	for (const auto& dir : fs::directory_iterator(flickDir)) {
		if (dir.is_directory()) {
			std::string dirPath = dir.path().string();
			if (dirPath.find("_internal") != std::string::npos)
				continue;
			flickPaths.push_back(dirPath);
		}
	}

	// Check which ones are missing in PATH
	std::string currentPath = getenv("PATH");
	std::vector<std::string> missingPaths;
	for (const auto& path : flickPaths) {
		if (currentPath.find(path) == std::string::npos) {
			missingPaths.push_back(path);
		}
	}

	if (missingPaths.empty()) {
		Log::log("All paths from Flick are already in PATH", Type::E_DISPLAY);
	} else {
		Log::log("Some paths from Flick are missing in PATH. Adding permanently to ~/.bashrc", Type::E_WARNING);
		Flick::addToPathPermanent(missingPaths);
	}
}

void Flick::setupVcpkg(const std::string& home, bool& is_install)
{
	std::string processLog{};
	if (fs::exists(std::string(home) + "/vcpkg")) {
		Log::log("VCPKG already installed on your system");
		return;
	}
	try {
		std::vector<std::string> args{"git", "clone", "https://github.com/microsoft/vcpkg.git", std::string(home) + "/vcpkg"};
		if (ProcessManager::startProcess(args, processLog, "Installing VCPKG From Github", false) != 0) {
			Log::log("failed to clone vcpkg", Type::E_ERROR);
			return;
		}
	} catch (std::exception& e) {
		Log::log(e.what(), Type::E_ERROR);
		return;
	};
#ifdef _WIN32
	std::string cmd{"setx VCPKG_ROOT " + home + "\\vcpkg"};
	addToPathPermanentWindows(home + "\\vcpkg");
	system(cmd.c_str());
	std::vector<std::string> args{home + "\\vcpkg\\bootstrap-vcpkg.bat"};
	if (!ProcessManager::startProcess(args, processLog, "Setting up vcpkg ...", false))
		return;
#else
	std::vector<std::string> args{home + "/vcpkg/bootstrap-vcpkg.sh"};
	if (!ProcessManager::startProcess(args, processLog, "Setting up vcpkg ..."))
		return;
	std::string bashrc = std::string("/home/") + getenv(USERNAME) + "/.bashrc";
	std::fstream file(bashrc.c_str(), std::ios::app);
	if (file.is_open()) {
		file << "export VCPKG_ROOT=" << home + "/vcpkg" << std::endl;
		file << "export PATH=$VCPKG_ROOT:$PATH" << std::endl;
		file.close();
	} else {
		Log::log("failed to open ~/.bashrc file!\n", Type::E_ERROR);
		return;
	}
#endif
	is_install = true;
};
// installing dev tools
void Flick::installTools(bool& isInstallationComplete)
{
#ifdef _WIN32
	namespace fs = std::filesystem;
	Log::log("This will install Clang-LLVM Toolchain with cmake,  ninja and vcpkg package manager from Github,\nAre you sure you "
		 "want to "
		 "continue??[y/n]",
		 Type::E_DISPLAY);
	char c;
	while (true) {

		fscanf(stdin, "%c", &c);
		if (c != '\n')
			break;
	};
	if (tolower(c) != 'y')
		exit(0);
	std::string home = getenv(USERNAME);
	if (!home.c_str())
		return;
	home += "\\flick";
	// system(("start " + std::string(VS_URL)).c_str());
	// Log::log("Make sure you download Desktop Development in C++ from Visual Studio Installer", Type::E_WARNING);
	Downloader::download(std::string(COMPILER_URL), home + "\\compiler.tar.xz");
	Downloader::download(std::string(CMAKE_URL), home + "\\cmake.zip");
	Downloader::download(std::string(NINJA_URL), home + "\\ninja.zip");
	Log::log(fmt::format("Extracting file at '{}'", home.c_str()));
	Unzip::unzip(home + "\\compiler.tar.xz", home);
	Unzip::unzip(home + "\\cmake.zip", home);
	Unzip::unzip(home + "\\ninja.zip", home);
	Log::log("removing downloaded archives...", Type::E_DISPLAY);
	fs::remove((home + "\\compiler.tar.xz"));
	fs::remove((home + "\\cmake.zip"));
	fs::remove((home + "\\ninja.zip"));
	if (!fs::exists(fs::path("C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe"))) {

		Downloader::download(std::string(VS_BUILD_TOOLS_INSTALLER_URL), home + "\\vs.exe");
		Downloader::download(/*Fix */, home + "\\flick.vsconfig");
		if (system((home + "\\vs.exe --quiet --wait --config " + home + "\\flick.vsconfig").c_str())) {
			Log::log("installing Visual Studio C++ Build Tools failed!", Type::E_ERROR);
		} else {
			Log::log("Visual Studio C++ Build Tools has been installed!", Type::E_DISPLAY);
			fs::remove((home + "\\vs.exe"));
		};
	} else {
		Log::log("Visual Studio Installer already installed on your system install buildtools now", Type::E_WARNING);
	}
	isInstallationComplete = true;
	addToPathWin();
	home = getenv(USERNAME);
	setupVcpkg(home, isInstallationComplete);
#else
#define DISTRO_INFO "/etc/os-release"

	Log::log("Install C++ clang Compiler and build tools using ex.[ubuntu]sudo apt install git ninja-build cmake clang clang-tools",
		 Type::E_WARNING);
	addToPathUnix();
	std::string home = "/home/";
	home += getenv(USERNAME);
	setupVcpkg(home, isInstallationComplete);
#endif
};

void Flick::setup()
{
	if (system("git --version") == 0) {
		onSetup();
	} else {
		Log::log("git is not installed", Type::E_ERROR);
		return;
	};
#ifdef _WIN32
	Log::log("[Attention] On Windows in order to use vcpkg packages without any issue please install Visual Studio Desktop Developement with C++",
		 Type::E_WARNING);
#else
	Log::log("[Attention] On Linux in order to use vcpkg packages without any issue please install pkgcofig, clang, lldb", Type::E_WARNING);
#endif // _WIN32
};

// creating packaged build [with installer for windows] using cpack
void Flick::createInstaller()
{
	for (auto& arg : mArgs) {
		if (arg.find("--static") != std::string::npos) {
			VCPKG_TRIPLET = getStandaloneTriplet();
			break;
		}
	};
	std::vector<std::string> args{"-Bbuild/release", "-DCMAKE_BUILD_TYPE=Release", "--preset=" + std::string(VCPKG_TRIPLET)};
	if (executeCMake(args)) {
		Log::log("Please First fix all the errors", Type::E_ERROR);
		return;
	}
	if (!release()) {
		Log::log("Please First fix all the errors", Type::E_ERROR);
		return;
	};
	if (!system("cd build/release && cpack"))
		return;
	std::ofstream file;
	file.open("CMakeLists.txt", std::ios::app);
	if (file.is_open()) {
		file << CPACK_CODE;
		file.close();
		if (!fs::exists("License.txt")) {
			ProjectGenerator::generateLicenceFile(mUserInfo);
		};
		if (system("cd build/release && cpack"))
			Log::log("CPack added to cmake run 'flick createinstaller' command again", Type::E_DISPLAY);
	} else {
		Log::log("Something went wrong :(\n", Type::E_ERROR);
	}
};

// running utest
void Flick::test()
{
	UnitTester tester(mUserInfo);
	tester.setupUnitTestingFramework();
	if (!tester.runUnitTesting(mArgs))
		return;
#ifdef _WIN32
	system(fmt::format(".\\build\\{}\\tests\\tests.exe", VCPKG_TRIPLET).c_str());
#else
	system(fmt::format("./build/{}/tests/tests", VCPKG_TRIPLET).c_str());
#endif
};

// TODO
// implementation is buggy right now will fix later
bool Flick::onSetup()
{
	bool isInstallationComplete{false};
	namespace fs = std::filesystem;
#ifdef _WIN32
	std::string home = getenv(USERNAME);
#else
	std::string home{"/home/"};
	home += getenv(USERNAME);
#endif
	if (!home.c_str())
		return false;
	std::fstream file;
#ifdef _WIN32
	if (!fs::create_directory(home + "\\flick")) {
		Log::log("flick dir alread exist", Type::E_WARNING);
	};
	file.open((home + "\\flick\\.cconfig").c_str(), std::ios::in);
	if (file.is_open()) {
		file >> isInstallationComplete;
		file.close();
		if (isInstallationComplete) {
			Log::log("Compiler is already installed if you think you messed up with Flick installation please use this command Flick fix",
				 Type::E_WARNING);
			return true;
		}
	} else {
		file.open((home + "\\flick\\.cconfig").c_str(), std::ios::out);
		if (file.is_open()) {
			installTools(isInstallationComplete);
			file << isInstallationComplete;
			file.close();
			addToPathWin();
			return true;
		}
	}
	if (!isInstallationComplete) {
		installTools(isInstallationComplete);
		file.open((home + "\\flick\\.cconfig").c_str(), std::ios::out);
		if (file.is_open()) {
			file << isInstallationComplete;
			file.close();
			addToPathWin();
			return true;
		};
		return false;
	};
	return true;
#else
	if (!fs::create_directory(home + "/flick")) {
		Log::log("flick dir alread exist", Type::E_WARNING);
	} else {
		Log::log(fmt::format("Creating Flick dir at {}", home.c_str()));
	};

	file.open((home + "/flick/.cconfig").c_str(), std::ios::in);
	if (file.is_open()) {
		file >> isInstallationComplete;
		file.close();
		if (isInstallationComplete) {
			Log::log("Compiler is already installed if you think you messed up with Flick installation please use this command Flick fix",
				 Type::E_WARNING);
			return true;
		}
	} else {
		Log::log("config file doesn't exist, creating one", Type::E_ERROR);
	};

	installTools(isInstallationComplete);

	file.open((home + std::string("/flick/.cconfig")).c_str(), std::ios::out);
	if (file.is_open()) {
		Log::log("writing to config file!", Type::E_DISPLAY);
		file << isInstallationComplete;
		file.close();
		Log::log("done!");
		addToPathUnix();
		return true;
	} else {
		Log::log("failed to write config file", Type::E_ERROR);
		return false;
	};

#endif
}
// TODO
// remove the ~/Flick and reinstall the Flick again with all the tools like cmake,g++ compiler,ninja,nsis
void Flick::fixInstallation()
{
	Log::log("Cleaning flick installation", Type::E_WARNING);
// TODO
#ifdef _WIN32
	std::string flickPath{};
#else
	std::string flickPath = "/home/";
#endif
	flickPath += getenv(USERNAME);
	flickPath += "/flick";
	auto path = fs::path(flickPath);
	for (const auto& dir : fs::directory_iterator(path)) {
		if (dir.is_directory()) {
			fs::remove_all(dir);
			Log::log(fmt::format("\033[95m{}\033[0m removed.", dir.path().string()), Type::E_WARNING);
		}
	}
	bool status{};
	installTools(status);
};

// cross-platform : creating processs to start the update
void createProcess(const std::string& path)
{
#ifdef _WIN32
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	if (CreateProcessA(path.c_str(),       // Path to updater executable
			   NULL,	       // Command line arguments
			   NULL,	       // Process handle not inheritable
			   NULL,	       // Thread handle not inheritable
			   FALSE,	       // No handle inheritance
			   CREATE_NEW_CONSOLE, // Run without opening a console window
			   NULL,	       // Use parent's environment
			   NULL,	       // Use parent's starting directory
			   &si,		       // Pointer to STARTUPINFO
			   &pi		       // Pointer to PROCESS_INFORMATION
			   )) {
		// Close process and thread handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		Log::log("Updater started successfully.", Type::E_DISPLAY);
	} else {
		Log::log("Failed to start updater!", Type::E_ERROR);
		if (GetLastError() == 740) {
			Log::log("Please try run this command with administrator privileges%s\n", Type::E_WARNING);
		} else {
			Log::log("unkown error occured!", Type::E_ERROR);
		};
	}
#else
	Log::log("Impementation is still in development for linux", Type::E_WARNING);
	// Linux implementation using fork() and exec()
	pid_t pid = fork(); // Create a new process

	if (pid < 0) {
		Log::log("Fork failed!", Type::E_ERROR);
		return;
	}

	if (pid == 0) {
		// This is the child process
		// Execute the update tool
		if (execlp(path.c_str(), path.c_str(), (char*)NULL) == -1) {
			Log::log("Failed to start update tool", Type::E_ERROR);
			return;
		};
	} else {
		// Parent process - can optionally wait for the child to finish or log the success
		Log::log("Updater started successfully in the background.", Type::E_DISPLAY);
		return;
	}
#endif
}
// download the utool from github if it's not already there then download the latest build from github
void Flick::update()
{
	namespace fs = std::filesystem;
#ifdef _WIN32
	std::string Flick = getenv(USERNAME);
#else
	std::string Flick{"/home/"};
	Flick += getenv(USERNAME);
#endif
#ifdef _WIN32
	Flick += "\\flick";
	std::string source{Flick + "\\utool.exe"};
#else
	Flick += "/flick";
	std::string source{Flick + "/utool"};
#endif
	Log::log("Updating Flick...", Type::E_DISPLAY);
	if (fs::exists(source)) // if utool is present in ~/Flick directory then start the utool if not download the utool first
	{
		createProcess(source); // Windows//starting process parent-less which will start utool so Flick will shutdown and then utool override
				       // the Flick.exe
	} else {
		Log::log("Could not find updater tool,you have to re-install the flick!", Type::E_ERROR);
	};
};
// TODO
void Flick::debug()
{
	if (!compile())
		return;
#ifdef _WIN32
	system(("lldb ./build/debug/" + mProjectSetting.getProjectName() + ".exe").c_str());
#else
	system(("lldb ./build/debug/" + mProjectSetting.getProjectName()).c_str());
#endif
};
// TODO
// this is actually useless for now but will add usefull stuff to it in future
bool Flick::release()
{

	namespace fs = std::filesystem;
	std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
	// auto formatedString = fmt::format("Threads in use : {}", cpuThreads.c_str());
	// Log::log(formatedString, Type::E_DISPLAY);
	for (auto& arg : mArgs) {
		if (arg.find("--static") != std::string::npos) {
			VCPKG_TRIPLET = getStandaloneTriplet();
			break;
		}
	};
	if (!fs::exists(fs::current_path().string() + "/build/release")) {
		std::vector<std::string> args{"-Bbuild/release", "-DCMAKE_BUILD_TYPE=Release", "--preset=" + std::string(VCPKG_TRIPLET)};
		if (!executeCMake(args)) {
			return false;
		}; // TODO
		// run ninja
		std::string pLog{};
		args.clear();
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/release");
		args.push_back("-j" + cpuThreads);
		if (ProcessManager::startProcess(args, pLog, "Compiling this may take minutes") ==
		    0) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");
			return true;
		} else {
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	} else {
		std::string pLog{};
		std::vector<std::string> args{};
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/release");
		args.push_back("-j" + cpuThreads);
		// run ninja
		if (ProcessManager::startProcess(args, pLog, "Compiling this may take minutes") ==
		    0) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");

			return true;
		} else {
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	}
	return false;
};
// for generating vscode intelligence
// everytime user run this command it's will override everything in c_cpp_properties.json


// it will simply delete the whole build folder and compile the project again
void Flick::reBuild()
{
	for (auto& arg : mArgs) {
		if (arg.find("--static") != std::string::npos) {
			VCPKG_TRIPLET = getStandaloneTriplet();
			break;
		}
	};
	namespace fs = std::filesystem;
	try {
		fs::remove_all("build");
		std::vector<std::string> args{"-DCMAKE_BUILD_TYPE=Debug", "--preset=" + std::string(VCPKG_TRIPLET)};
		executeCMake(args);
		compile();
	} catch (std::exception& e) {
		Log::log(e.what(), Type::E_ERROR);
	};
};

void Flick::buildDeps()
{
	// building cmake external libraries
	Deps deps;
	deps.buildDeps();
}
void Flick::addDeps()
{
	std::string name{}, version{}, project{};
	bool bUseLatest{};
	bool bUpdateBaseline{};
	if (mArgs.size() < 3) {
		Log::log("No Package \033[95mname\033[0m given", Type::E_ERROR);
		return;
	};
	project = mProjectSetting.getProjectName();
	for (const auto& arg : mArgs) {
		if (arg.find("--package=") != std::string::npos) {
			// Extract package name after "--package="
			name = arg.substr(arg.find("=") + 1);
		} else if (arg.find("--version=") != std::string::npos) {
			// Extract version number after "--version="
			version = arg.substr(arg.find("=") + 1);
		} else if (arg.find("--project=") != std::string::npos) {
			// Extract project name after "--project="
			project = arg.substr(arg.find("=") + 1);
		} else if (arg.find("--update-base-line") != std::string::npos) {
			bUpdateBaseline = true;
		} else if (arg.find("--latest") != std::string::npos) {
			bUseLatest = true;
		}
	}
	if (name.empty()) {
		Log::log("Uses : flick install \033[95m--package=boost-process --version=1.85.0 --project=demo --update-base-line\033[0m",
			 Type::E_ERROR);
		Log::log("Other args are optional but package name must be provided: flick install \033[95m--package=fmt\033[0m", Type::E_ERROR);
		return;
	}
	if (bUseLatest) {
		Log::log("fetching latest packages from \033[95mvcpkg\033[0m", Type::E_WARNING);
		std::string log{};
		std::string vcpkg{std::getenv("VCPKG_ROOT")};
		if (vcpkg.length() <= 0) {
			Log::log("\033[95mVCPKG_ROOT\033[0m is not Set!", Type::E_ERROR);
			return;
		}
		std::vector<std::string> args{"git", "-C", vcpkg, "pull"};
		ProcessManager::startProcess(args, log, "", false);
	}
	Deps deps;
	if (deps.addDeps(name, version, bUpdateBaseline)) {
		std::string vcpkgLog{};
		for (auto& arg : mArgs) {
			if (arg.find("--static") != std::string::npos) {
				VCPKG_TRIPLET = getStandaloneTriplet();
				break;
			}
		};
		if (!deps.installDeps(vcpkgLog, VCPKG_TRIPLET)) {
			return;
		};
		deps.updateCMakeFile(vcpkgLog, project, name);
	}
}
void Flick::genCMakePreset()
{
	Log::log("Please Choose your Programming language \033[95mc/cc\033[0m default=cc,q=quit > ", Type::E_DISPLAY, "");
	std::string input{};
	std::getline(std::cin, input);
	Language lang{};
	if (input.empty())
		lang = Language::CXX;
	else if (input == "c")
		lang = Language::C;
	else if (input == "cc")
		lang = Language::CXX;
	ProjectGenerator::generateCMakePreset(lang);
};

void Flick::createSubProject()
{
	if (mArgs.size() < 3) {
		Log::log("No subproject name? What do you want me to do, \033[95mguess\033[0m?", Type::E_ERROR);
		return;
	}
	auto info = readuserInput();
	ProjectGenerator generator{};
	generator.setProjectSetting(mProjectSetting, info.second, info.first);
	generator.generateSubProject(mArgs.at(2));
}

void Flick::listPackages()
{
	std::ifstream in("vcpkg.json");
	if (in.is_open()) {
		nlohmann::json j;
		in >> j;
		if (!j.contains("dependencies")) {
			Log::log("No dependencies are being used");
			return;
		}
		const auto dependencies = j["dependencies"];
		Log::log(fmt::format("total dependencies: \033[95m{}\033[0m", dependencies.size()));
		for (const auto& dep : dependencies) {
			Log::log(fmt::format("\033[95m------{} \033[0mversion : {}", dep.get<std::string>(),
					     [&j](const std::string& dep) -> std::string {
						     if (!j.contains("overrides"))
							     return "latest";
						     const auto overrides = j["overrides"];
						     for (const auto& override : overrides) {
							     if (override.contains("name") && override["name"].get<std::string>() == dep) {
								     return override.contains("version") ? override["version"].get<std::string>()
													 : "latest";
							     };
						     };
						     return "latest";
					     }(dep)),
				 Type::E_NONE);
		}
	}
	in.close();
}

void Flick::generateClangFiles(const std::string& projectDir)
{
	std::string path = projectDir + "/.clang-format";
	std::ofstream file(path);
	if (!file.is_open()) {
		Log::log("Failed to create .clang-format file", Type::E_ERROR);
		return;
	}
	file << CLANG_FORMAT;
	file.close();
	Log::log(".clang-format generated");
	path = projectDir + "/.clang-tidy";
	file.open(path);
	if (!file) {
		Log::log("Failed to create .clang-tidy file", Type::E_ERROR);
		return;
	}

	file << CLANG_TIDY;
	file.close();
	Log::log(".clang-tidy generated");
	path = projectDir + "/.clangd";
	file.open(path);
	if (!file) {
		Log::log("Failed to create .clangd file", Type::E_ERROR);
		return;
	}

	file << CLANGD;
	file.close();
	Log::log(".clangd generated");
	path = projectDir + "/.editorconfig";
	file.open(path);
	if (!file) {
		Log::log("Failed to create .editorconfig file", Type::E_ERROR);
		return;
	}

	file << EDITOR_CONFIG;
	file.close();
	Log::log(".editorconfig generated");
}

void Flick::reinit(const std::string& projectDir)
{
	generateClangFiles(projectDir);
};
