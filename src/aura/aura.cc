#include <thread>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <format>
#include <chrono>
#include <aura/aura.hpp>
#include <projectgenerator/projectgenerator.h>
#include <constants/colors.hpp>
#include <constants/constant.hpp>
#include <downloader/downloader.h>
#include <rt/rt.h>
#include <log/log.h>
#include <deps/deps.h>
#include <utils/utils.h>
#include <userinfo/userinfo.h>
#include <unittester/unittester.h>
#include <regex>
#include <reproc++/reproc.hpp>

namespace fs = std::filesystem;
Aura::Aura(const std::vector<std::string> &args)
{
	mArgs = args;
	std::string cmd{args.at(1)};
	UserInfo::readUserInfoFromConfigFile(&this->mUserInfo);
	if (cmd == "create")
	{
		if (args.size() < 3)
		{
			Log::log("No name for project! are you serious?", Type::E_ERROR);
			exit(0);
		};
		auto projectName{mArgs.at(2)};
		std::regex pattern("^[a-z][a-z0-9-]*$");
		// if (std::any_of(project_name.begin(), project_name.end(), [](const char &c)
		// 				{ return !((c >= 'a' && c <= 'z') || isdigit(c) || c == '-'); }))
		if (!std::regex_match(projectName, pattern))
		{
			Log::log("Project name must be in lowercase with no space and special characters allowed name : ^[a-z][a-z0-9-]*$", Type::E_ERROR);
			std::exit(0);
		};
		mProjectSetting.set(mArgs.at(2));
		return;
	};
	if (cmd != "setup" && cmd != "doctor" && cmd != "update" && cmd != "builddeps")
		ProjectGenerator::readProjectSettings(&this->mProjectSetting);
};
Aura::~Aura() {

};

void Aura::createNewProject()
{
	ProjectGenerator generator{};
	Log::log("Please Choose your Programming language c/cc default=cc,q=quit > ", Type::E_DISPLAY, "");
	std::string input{};
	std::getline(std::cin, input);
	Language lang{};
	if (input.empty())
		lang = Language::CXX;
	else if (input == "c")
		lang = Language::C;
	else if (input == "cc")
		lang = Language::CXX;
	else if (input == "q")
	{
		mRt.~RT();
		std::exit(0);
	}
	generator.setProjectSetting(mProjectSetting, lang);
	generator.generate();
};

bool Aura::executeCMake(const std::string &additionalCMakeArg)
{
	std::string cmd{"cmake -S . -G \"Ninja\" "};
	cmd += additionalCMakeArg;
	return system(cmd.c_str()) == 0;
};

// TODO : add compile option
bool Aura::compile()
{
	// Temp Soln

	namespace fs = std::filesystem;
	std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
	auto formatedString = std::format("Threads in use : {}", cpuThreads.c_str());
	Log::log(formatedString, Type::E_DISPLAY);
	if (!fs::exists(fs::current_path().string() + "/build/debug"))
	{
		for (auto &arg : mArgs)
		{
			if (arg.find("--nostatic") != std::string::npos)
			{
				VCPKG_TRIPLET = "default";
				break;
			}
		};
		// run cmake
		Log::log("Compile Process has been started...", Type::E_DISPLAY);
		executeCMake(std::string("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug --preset=") + std::string(VCPKG_TRIPLET)); // TODO
		// run ninja
		if (!system(("cmake --build build/debug -j" + cpuThreads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");
			return true;
		}
		else
		{
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	}
	else
	{
		// run ninja
		if (!system(("cmake --build build/debug -j" + cpuThreads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");

			return true;
		}
		else
		{
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	}
};
//
void Aura::run()
{
	std::string run{};
#ifdef _WIN32
	run += ".\\build\\debug\\";
	run += mProjectSetting.getProjectName();
	run += ".exe";
#else
	run += "./build/debug/";
	run += _project_setting.getProjectName();
#endif // _WIN32
	bool is_arg{false};
	for (auto &arg : mArgs)
	{
		if (arg.find("--args") != std::string::npos)
		{
			is_arg = true;
			continue;
		}
		else if (arg.find("--nostatic")!=std::string::npos)
		{
			is_arg = false;
			continue;
		};
		if (is_arg)
		{
			run += " ";
			run+=arg;
		};
	}
	if (system(run.c_str()))
	{
		Log::log("Maybe You should Compile First Before run or You have Permission to "
				 "execute program!",
				 Type::E_ERROR);
	};
}

//
void Aura::build()
{
	if (!this->compile())
		return;
	this->run();
}

//
void Aura::addToPathWin()
{
#ifdef _WIN32
	namespace fs = std::filesystem;
	std::string aura{getenv(USERNAME)};
	aura += "\\.aura";
	std::string source{fs::current_path().string() + "\\aura.exe"};
	std::string destination{(aura + "\\aura.exe").c_str()};
	if (source.compare(destination) != 0)
	{
		for (auto &dll : fs::directory_iterator(fs::current_path()))
		{
			if (dll.is_directory())
				continue;
			if (dll.path().filename().string().find(".dll") != std::string::npos)
			{
				printf("%sCopying %s to %s%s\n", GREEN, dll.path().filename().string().c_str(), aura.c_str(), WHITE);
				// if (fs::copy_file(dll.path(), aura, fs::copy_options::update_existing))
				// {
				// 	printf("%s copied to %s\n", dll.path().filename().string().c_str(), aura.c_str());
				// }
			}
		}
		if (!fs::exists(source))
		{
			Log::log("aura doesn't exist in current dir", Type::E_WARNING);
		}
		else
		{
			printf("%sCopying aura into %s%s\n", GREEN, aura.c_str(), WHITE);
			if (fs::copy_file(source, destination, fs::copy_options::update_existing))
			{
				printf("%s copied to %s\n", source.c_str(), destination.c_str());
			}
			else
			{
				Log::log("error while copying aura.exe into aura directory!", Type::E_ERROR);
			};
		}
	}
	std::string path{aura + ";"};

	for (const auto &dir : fs::directory_iterator(aura))
	{
		if (dir.is_directory())
		{
			if (dir.path().string().find("_internal") != std::string::npos)
				continue;
			if (dir.path().string().find("vcpkg") != std::string::npos)
			{
				path += dir.path().string() + ";";
				continue;
			}
			if (dir.path().string().find("nsis") != std::string::npos)
			{
				path += dir.path().string() + ";";
				path += (dir.path().string() + "\\Bin;");
				continue;
			};

			path += dir.path().string();
			path += "\\bin;";
		}
	};

	std::string env{getenv("path")};
	// Split path by semicolons and check each path individually
	std::istringstream pathStream(path);
	std::string singlePath;
	bool found = true;
	std::string newPath{};
	while (std::getline(pathStream, singlePath, ';'))
	{
		if (env.find(singlePath) == std::string::npos)
		{
			found = false;
			newPath += singlePath;
			newPath += ";";
		}
	}

	if (found)
	{
		Log::log("All paths from aura are in PATH", Type::E_DISPLAY);
	}
	else
	{
		Log::log("Some paths from aura are missing in PATH adding these entries into path make sure to restart your shell after that", Type::E_WARNING);
		pathStream.clear();
		pathStream.str(newPath);
		std::string tempStr{};
		while (std::getline(pathStream, tempStr, ';'))
		{
			std::cout << tempStr << "\n";
		};
		system(("setx PATH \"%PATH%;" + newPath + "\"").c_str());
	}
#endif
}
//
void Aura::addToPathUnix()
{

	namespace fs = std::filesystem;
	std::string aura{"/home/"};
	aura += getenv(USERNAME);
	aura += "/.aura";
	std::string source{fs::current_path().string() + "/aura"};
	std::string destination{(aura + "/aura").c_str()};
	if (source.compare(destination) != 0)
	{
		if (!fs::exists(source))
		{
			Log::log("aura doesn't exist in current dir", Type::E_ERROR);
		}
		else
		{
			printf("%sCopying aura into %s%s\n", GREEN, destination.c_str(), WHITE);
			fs::remove(destination);
			if (fs::copy_file(source, destination, fs::copy_options::overwrite_existing))
			{
				printf("%s copied to %s\n", source.c_str(), destination.c_str());
			}
			else
			{
				Log::log("error while copying aura.exe into aura directory!", Type::E_ERROR);
			};
		}
	}
	std::string path{aura + ";"};
	for (const auto &dir : fs::directory_iterator(aura))
	{
		if (dir.is_directory())
		{
			if (dir.path().string().find("_internal") != std::string::npos)
				continue;

			path += dir.path().string();
			path += ";";
		};
	};
	std::string env{getenv("PATH")};
	// Split path by semicolons and check each path individually
	std::istringstream pathStream(path);
	std::string singlePath;
	bool found = true;
	std::string newPath{};
	while (std::getline(pathStream, singlePath, ';'))
	{
		std::cout << singlePath << "\n";
		if (env.find(singlePath) == std::string::npos)
		{
			found = false;
			newPath += singlePath;
			newPath += ":";
		}
	}
	if (!newPath.empty() && newPath.back() == ':')
	{
		newPath.pop_back();
	}
	if (found)
	{
		Log::log("All paths from aura are in PATH", Type::E_DISPLAY);
	}
	else
	{
		Log::log("Some paths from aura are missing in PATH adding these entries into path make sure to restart your shell after that", Type::E_WARNING);
		pathStream.clear();
		pathStream.str(newPath);
		std::string tempStr{};
		std::string bashrc = std::string("/home/") + getenv(USERNAME) + "/.bashrc";
		std::fstream file(bashrc.c_str(), std::ios::app);
		if (file.is_open())
		{
			file << "export PATH=\"$PATH:" << newPath << "\"\n";
			file.close();
		}
		else
		{
			Log::log("failed to open ~/.bashrc file!\n", Type::E_ERROR);
			return;
		}
	};
};

void Aura::setupVcpkg(const std::string &home, bool &is_install)
{
	try
	{
		system((std::string("git clone https://github.com/microsoft/vcpkg.git ") + std::string(home) + "/vcpkg").c_str());
	}
	catch (std::exception &e)
	{
		Log::log(e.what(), Type::E_ERROR);
		return;
	};
#ifdef _WIN32
	std::string cmd{"setx VCPKG_ROOT " + home + "\\vcpkg"};
	system(cmd.c_str());
	system(("cd " + home + "\\vcpkg &&" + " .\\bootstrap-vcpkg.bat").c_str());
#else
	system(("cd " + home + "\\vcpkg &&" + " ./bootstrap-vcpkg.sh").c_str());
	std::string bashrc = std::string("/home/") + getenv(USERNAME) + "/.bashrc";
	std::fstream file(bashrc.c_str(), std::ios::app);
	if (file.is_open())
	{
		file << "export VCPKG_ROOT=" << home + "/vcpkg" << std::endl;
		file << "export PATH=$VCPKG_ROOT:$PATH" << std::endl;
		file.close();
	}
	else
	{
		Log::log("failed to open ~/.bashrc file!\n", Type::E_ERROR);
		return;
	}
#endif
	is_install = true;
};
// installing dev tools
void Aura::installTools(bool &isInstallationComplete)
{
#ifdef _WIN32
	namespace fs = std::filesystem;
	Log::log("This will install Clang-LLVM Toolchain with cmake,  ninja and vcpkg package manager from Github,\nAre you sure you "
			 "want to "
			 "continue??[y/n]",
			 Type::E_DISPLAY);
	char c;
	while (true)
	{

		fscanf(stdin, "%c", &c);
		if (c != '\n')
			break;
	};
	if (tolower(c) != 'y')
		exit(0);
	std::string home = getenv(USERNAME);
	if (!home.c_str())
		return;
	home += "\\.aura";
	// system(("start " + std::string(VS_URL)).c_str());
	// Log::log("Make sure you download Desktop Development in C++ from Visual Studio Installer", Type::E_WARNING);
	Downloader::download(std::string(COMPILER_URL), home + "\\compiler.tar.xz");
	Downloader::download(std::string(CMAKE_URL), home + "\\cmake.zip");
	Downloader::download(std::string(NINJA_URL), home + "\\ninja.zip");
	printf("%sextracting file at %s%s\n", BLUE, home.c_str(), WHITE);
	if (system((std::string("tar -xvjf ") + "\"" + home + "\\compiler.tar.xz\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\cmake.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\ninja.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	Log::log("removing downloaded archives...", Type::E_DISPLAY);
	fs::remove((home + "\\compiler.tar.xz"));
	fs::remove((home + "\\cmake.zip"));
	fs::remove((home + "\\ninja.zip"));
	Downloader::download(std::string(VS_BUILD_TOOLS_INSTALLER_URL),home+"\\vs.exe");
	Downloader::download("https://github.com/vishal-ahirwar/aura/blob/master/res/aura.vsconfig",home+"\\aura.vsconfig");
	if(system((home+"\\vs.exe --quiet --wait --config "+home+"\\aura.vsconfig").c_str())){
		Log::log("installing Visual Studio C++ Build Tools failed!",Type::E_ERROR);
	}else{
		Log::log("Visual Studio C++ Build Tools has been installed!",Type::E_DISPLAY);
		fs::remove((home + "\\vs.exe"));
	};
	isInstallationComplete = true;
	setupVcpkg(home, isInstallationComplete);
	addToPathWin();
#else
#define DISTRO_INFO "/etc/os-release"

	Log::log("Install C++ clang Compiler and build tools using ex.[ubuntu]sudo apt install git ninja-build cmake clang clang-tools", Type::E_WARNING);
	addToPathUnix();
	isInstallationComplete = true;

#endif
};

void Aura::setup()
{
	if (system("git --version") == 0)
	{
		onSetup();
	}
	else
	{
		Log::log("git is not installed", Type::E_ERROR);
		return;
	};
#ifdef _WIN32
	Log::log("[Attention] On Windows in order to use vcpkg packages without any issue please install Visual Studio Desktop Developement with C++", Type::E_WARNING);
#else
	Log::log("[Attention] On Linux in order to use vcpkg packages without any issue please install pkgcofig, clang, lldb", Type::E_WARNING);
#endif // _WIN32
};

// creating packaged build [with installer for windows] using cpack
void Aura::createInstaller()
{
	for (auto &arg : mArgs)
	{
		if (arg.find("--nostatic") != std::string::npos)
		{
			VCPKG_TRIPLET = "default";
			break;
		}
	};
	if (!executeCMake("-Bbuild/release -DCMAKE_BUILD_TYPE=Release --preset=" + std::string(VCPKG_TRIPLET)))
	{
		Log::log("Please First fix all the errors", Type::E_ERROR);
		return;
	}
	if (!release())
	{
		Log::log("Please First fix all the errors", Type::E_ERROR);
		return;
	};
	if (!system("cd build/release && cpack"))
		return;
	std::ofstream file;
	file.open("CMakeLists.txt", std::ios::app);
	if (file.is_open())
	{
		file << CPACK_CODE;
		file.close();
		if (!fs::exists("License.txt"))
		{
			ProjectGenerator::generateLicenceFile(mUserInfo);
		};
		if (system("cd build/release && cpack"))
			Log::log("CPack added to cmake run 'aura createinstaller' command again",
					 Type::E_DISPLAY);
	}
	else
	{
		Log::log("Something went wrong :(\n", Type::E_ERROR);
	}
};

// running utest
void Aura::test()
{
	UnitTester tester(mUserInfo);
	tester.setupUnitTestingFramework();
	tester.runUnitTesting(mArgs);
#ifdef _WIN32
	system(".\\build\\tests\\tests.exe");
#else
	system("./build/tests/tests");
#endif
};

// TODO
// implementation is buggy right now will fix later
bool Aura::onSetup()
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
	if (!fs::create_directory(home + "\\.aura"))
	{
		Log::log(".aura dir alread exist", Type::E_WARNING);
	};
	file.open((home + "\\.aura\\.cconfig").c_str(), std::ios::in);
	if (file.is_open())
	{
		file >> isInstallationComplete;
		file.close();
		if (isInstallationComplete)
		{
			Log::log("Compiler is already installed if you think you messed up with aura installation please use this command aura fix", Type::E_WARNING);
			return true;
		}
	}
	else
	{
		file.open((home + "\\.aura\\.cconfig").c_str(), std::ios::out);
		if (file.is_open())
		{
			installTools(isInstallationComplete);
			file << isInstallationComplete;
			file.close();
			addToPathWin();
			return true;
		}
	}
	if (!isInstallationComplete)
	{
		installTools(isInstallationComplete);
		file.open((home + "\\.aura\\.cconfig").c_str(), std::ios::out);
		if (file.is_open())
		{
			file << isInstallationComplete;
			file.close();
			addToPathWin();
			return true;
		};
		return false;
	};
	return true;
#else
	if (!fs::create_directory(home + "/.aura"))
	{
		Log::log(".aura dir alread exist", Type::E_WARNING);
	}
	else
	{
		printf("%sCreating aura dir at %s %s\n", BLUE, home.c_str(), WHITE);
	};

	file.open((home + "/.aura/.cconfig").c_str(), std::ios::in);
	if (file.is_open())
	{
		file >> isInstallationComplete;
		file.close();
		if (isInstallationComplete)
		{
			Log::log("Compiler is already installed if you think you messed up with aura installation please use this command aura fix", Type::E_WARNING);
			return true;
		}
	}
	else
	{
		Log::log("config file doesn't exist, creating one", Type::E_ERROR);
	};

	installTools(isInstallationComplete);

	file.open((home + std::string("/.aura/.cconfig")).c_str(), std::ios::out);
	if (file.is_open())
	{
		Log::log("writing to config file!", Type::E_DISPLAY);
		file << isInstallationComplete;
		file.close();
		std::cout << "done!\n";
		addToPathUnix();
		return true;
	}
	else
	{
		Log::log("failed to write config file", Type::E_ERROR);
		return false;
	};

#endif
}
// TODO
// remove the ~/aura and reinstall the aura again with all the tools like cmake,g++ compiler,ninja,nsis
void Aura::fixInstallation() {
	// TODO
};

// cross-platform : creating processs to start the update
void createProcess(const std::string &path)
{
#ifdef _WIN32
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	if (CreateProcessA(
			path.c_str(),		// Path to updater executable
			NULL,				// Command line arguments
			NULL,				// Process handle not inheritable
			NULL,				// Thread handle not inheritable
			FALSE,				// No handle inheritance
			CREATE_NEW_CONSOLE, // Run without opening a console window
			NULL,				// Use parent's environment
			NULL,				// Use parent's starting directory
			&si,				// Pointer to STARTUPINFO
			&pi					// Pointer to PROCESS_INFORMATION
			))
	{
		// Close process and thread handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		Log::log("Updater started successfully.", Type::E_DISPLAY);
	}
	else
	{
		Log::log("Failed to start updater!", Type::E_ERROR);
		if (GetLastError() == 740)
		{
			Log::log("Please try run this command with administrator privileges%s\n", Type::E_WARNING);
		}
		else
		{
			Log::log("unkown error occured!", Type::E_ERROR);
		};
	}
#else
	Log::log("Impementation is still in development for linux", Type::E_WARNING);
	// Linux implementation using fork() and exec()
	pid_t pid = fork(); // Create a new process

	if (pid < 0)
	{
		Log::log("Fork failed!", Type::E_ERROR);
		return;
	}

	if (pid == 0)
	{
		// This is the child process
		// Execute the update tool
		if (execlp(path.c_str(), path.c_str(), (char *)NULL) == -1)
		{
			Log::log("Failed to start update tool", Type::E_ERROR);
			return;
		};
	}
	else
	{
		// Parent process - can optionally wait for the child to finish or log the success
		Log::log("Updater started successfully in the background.", Type::E_DISPLAY);
		return;
	}
#endif
}
// download the utool from github if it's not already there then download the latest build from github
void Aura::update()
{
	namespace fs = std::filesystem;
#ifdef _WIN32
	std::string aura = getenv(USERNAME);
#else
	std::string aura{"/home/"};
	aura += getenv(USERNAME);
#endif
#ifdef _WIN32
	aura += "\\.aura";
	std::string source{aura + "\\utool.exe"};
#else
	aura += "/.aura";
	std::string source{aura + "/utool"};
#endif
	Log::log("updating aura...", Type::E_DISPLAY);
	std::cout << source << "\n";
	if (fs::exists(source)) // if utool is present in ~/aura directory then start the utool if not download the utool first
	{
		createProcess(source); // Windows//starting process parent-less which will start utool so aura will shutdown and then utool override the aura.exe
	}
	else
	{
		Downloader::download(std::string(UPDATER_URL), source);
#ifndef _WIN32 // for linux we have to set permission for the newly downloaded file
		system(("chmod +x " + source).c_str());
#endif
		createProcess(source); // starting process parent-less which will start utool so aura will shutdown and then utool override the aura.exe
	};
};
// TODO
void Aura::debug()
{
	if (!compile())
		return;
#ifdef _WIN32
	system(("lldb ./build/debug/" + mProjectSetting.getProjectName() + ".exe").c_str());
#else
	system(("lldb ./build/debug/" + _project_setting.getProjectName()).c_str());
#endif
};
// TODO
// this is actually useless for now but will add usefull stuff to it in future
bool Aura::release()
{

	namespace fs = std::filesystem;
	std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
	auto formatedString = std::format("Threads in use : {}", cpuThreads.c_str());
	Log::log(formatedString, Type::E_DISPLAY);
	for (auto &arg : mArgs)
	{
		if (arg.find("--nostatic") != std::string::npos)
		{
			VCPKG_TRIPLET = "default";
			break;
		}
	};
	if (!fs::exists(fs::current_path().string() + "/build/release"))
	{
		// run cmake
		Log::log("Compile Process has been started...", Type::E_DISPLAY);
		executeCMake(std::string("-Bbuild/release -DCMAKE_BUILD_TYPE=Release --preset=") + std::string(VCPKG_TRIPLET)); // TODO
		// run ninja
		if (!system(("cmake --build build/release -j" + cpuThreads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");
			return true;
		}
		else
		{
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	}
	else
	{
		// run ninja
		if (!system(("cmake --build build/release -j" + cpuThreads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL");

			return true;
		}
		else
		{
			Log::log("BUILD FAILED", Type::E_ERROR);
			return false;
		}
	}
	return false;
};
// for generating vscode intelligence
// everytime user run this command it's will override everything in c_cpp_properties.json
void Aura::vsCode()
{
	namespace fs = std::filesystem;
	if (!fs::exists("vcpkg.json"))
	{
		Log::log("vscode config file generation failed", Type::E_ERROR);
		return;
	}
	if (fs::exists(".vscode"))
		Log::log(".vscode already exist!", Type::E_WARNING);
	else
		fs::create_directory(".vscode");
	std::ofstream file(".vscode/c_cpp_properties.json", std::ios::out);
	if (file.is_open())
	{
		file << VSCODE_CONFIG;
	}
	else
	{
		Log::log("failed to create .vscode/c_cpp_properties.json", Type::E_ERROR);
	}
	file.close();
}

// it will simply delete the whole build folder and compile the project again
void Aura::reBuild()
{
	for (auto &arg : mArgs)
	{
		if (arg.find("--nostatic") != std::string::npos)
		{
			VCPKG_TRIPLET = "default";
			break;
		}
	};
	namespace fs = std::filesystem;
	try
	{
		fs::remove_all("build");
		executeCMake("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug --preset=" + std::string(VCPKG_TRIPLET));
		compile();
	}
	catch (std::exception &e)
	{
		Log::log(e.what(), Type::E_ERROR);
	};
};

void Aura::refresh()
{
	for (auto &arg : mArgs)
	{
		if (arg.find("--nostatic") != std::string::npos)
		{
			VCPKG_TRIPLET = "default";
			break;
		}
	};
	executeCMake("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug --preset=" + std::string(VCPKG_TRIPLET));
};
void Aura::buildDeps()
{
	// building cmake external libraries
	Deps deps;
	deps.buildDeps();
}
void Aura::addDeps()
{
	if (mArgs.size() < 3)
	{
		Log::log("You have to provide github url! make sure it has CMakeList.txt", Type::E_ERROR);
		return;
	};
	Deps deps;
	if (deps.addDeps(mArgs.at(2)))
	{
		deps.updateCMakeFile(mArgs.at(2));
	}
	else
		Log::log("Failed to add " + mArgs.at(2) + "make sure you solve those errors or remove it from external directory!", Type::E_ERROR);
}
void Aura::genCMakePreset()
{
	Log::log("Please Choose your Programming language c/cc default=cc,q=quit > ", Type::E_DISPLAY, "");
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

void Aura::createSubProject()
{
	Log::log("SubProject Name [Press Enter To Cancel]> ", Type::E_DISPLAY, "");
	std::string input{};
	std::getline(std::cin, input);
	Language lang{};
	if (input.empty())
		return;
	Log::log("subproject name is "+input, Type::E_DISPLAY, "");
}
