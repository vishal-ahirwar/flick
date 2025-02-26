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
#include <json.hpp>
#include <deps/deps.h>
#include <utils/utils.h>
#include <userinfo/userinfo.h>
#include<unittester/unittester.h>
#ifdef _WIN32
#include <windows.h>
#define USERNAME "USERPROFILE"
#else
#include <unistd.h>
#define USERNAME "USER"
#endif

namespace fs = std::filesystem;
Aura::Aura(const std::vector<std::string> &args)
{
	_args = args;
	std::string cmd{args.at(1)};
	UserInfo::readUserInfoFromConfigFile(&this->_user_info);
	if (cmd == "create")
	{
		if (args.size() < 3)
		{
			Log::log("No name for project! are you serious?", Type::E_ERROR);
			exit(0);
		};
		std::time_t now = std::time(nullptr);
		std::string date{std::ctime(&now)};
		date.pop_back();
		_project_setting.set(_args.at(2), _user_info.getUserName(), date, std::string(CONFIG_CMAKE_ARGS));
		return;
	};
	if (cmd != "setup" && cmd != "fix" && cmd != "update" && cmd != "builddeps")
		ProjectGenerator::readProjectSettings(&this->_project_setting);
};
Aura::~Aura() {

};

void Aura::createNewProject()
{
	ProjectGenerator generator{};
	Log::log("Please Choose your Programming language [0.C]/[1.CXX]/[Default.CXX] : ", Type::E_DISPLAY);
	int input{getchar()};
	ProjectGenerator::Lang lang{};
	switch (input)
	{
	case 0:
	{
		lang = ProjectGenerator::Lang::C;
		break;
	};
	case 1:
	{
		lang = ProjectGenerator::Lang::CXX;
		break;
	};
	default:
	{
		lang = ProjectGenerator::Lang::CXX;
		break;
	};
	};
	generator.setProjectSetting(_project_setting, lang);
	generator.generate();
};

bool Aura::executeCMake(const std::string &additional_cmake_arg)
{
	std::string cmd{"cmake -S . -G \"Ninja\" "};
	cmd += additional_cmake_arg;
	return system(cmd.c_str()) == 0;
};

// TODO : add compile option
bool Aura::compile()
{
	// Temp Soln

	namespace fs = std::filesystem;
	std::string cpu_threads{std::to_string(std::thread::hardware_concurrency() - 1)};
	printf("%sThreads in use: %s%s\n", YELLOW, cpu_threads.c_str(), WHITE);
	if (!fs::exists(fs::current_path().string() + "/build/debug"))
	{
		// run cmake
		Log::log("Compile Process has been started...", Type::E_DISPLAY);
		executeCMake(std::string("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug ") + _project_setting.getCMakeArgs()); // TODO
		// run ninja
		if (!system(("ninja -C build/debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL", Type::E_DISPLAY);
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
		if (!system(("ninja -C build/debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL", Type::E_DISPLAY);

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
	// printf("%s%s: \n%s", YELLOW, projectName.c_str(),WHITE);
#ifdef _WIN32
	run += ".\\build\\debug\\";
	run += _project_setting.getProjectName();
	run += ".exe";
#else
	run += "./build/debug/";
	run += _project_setting.getProjectName();
#endif // _WIN32
	for (int i = 2; i < _args.size(); ++i)
	{
		run += " ";
		run += _args.at(i);
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

// installing dev tools
void Aura::installEssentialTools(bool &isInstallationComplete)
{
#ifdef _WIN32
	namespace fs = std::filesystem;
	Log::log("This will install Clang-LLVM Toolchain with cmake and ninja from Github,\nAre you sure you "
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
	Downloader::download(std::string(COMPILER_URL_64BIT), home + "\\compiler.zip");
	Downloader::download(std::string(CMAKE_URL_64BIT), home + "\\cmake.zip");
	Downloader::download(std::string(NINJA_URL_64BIT), home + "\\ninja.zip");
	Downloader::download(std::string(NSIS_URL), home + "\\nsis.zip");
	printf("%sunzipping file at %s%s\n", BLUE, home.c_str(), WHITE);
	if (system((std::string("tar -xf ") + "\"" + home + "\\compiler.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\cmake.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\ninja.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\nsis.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	Log::log("removing downloaded archives...", Type::E_DISPLAY);
	fs::remove((home + "\\compiler.zip"));
	fs::remove((home + "\\cmake.zip"));
	fs::remove((home + "\\ninja.zip"));
	fs::remove((home + "\\nsis.zip"));
	isInstallationComplete = true;
	addToPathWin();
#else
#define DISTRO_INFO "/etc/os-release"

	Log::log("Install C++ clang Compiler and build tools using ex.[ubuntu]sudo apt install git ninja-build cmake clang clang-tools", Type::E_WARNING);
	addToPathUnix();
	isInstallationComplete = true;

#endif
};
//
void Aura::setup()
{
	onSetup();
};


// creating packaged build [with installer for windows] using cpack
void Aura::createInstaller()
{
	if (!executeCMake("-Bbuild/release -DCMAKE_BUILD_TYPE=Release " + _project_setting.getCMakeArgs()))
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
		if(!fs::exists("License.txt"))
		{
			UnitTester utester(_user_info);
			utester.generateLicenceFile();
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
	UnitTester tester(_user_info);
	tester.setupUnitTestingFramework();
	tester.runUnitTesting();
#ifdef _WIN32
	system(".\\build\\debug\\tests.exe");
#else
	system("./build/debug/tests");
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
			installEssentialTools(isInstallationComplete);
			file << isInstallationComplete;
			file.close();
			return true;
		}
	}
	if (!isInstallationComplete)
	{
		installEssentialTools(isInstallationComplete);
		file.open((home + "\\.aura\\.cconfig").c_str(), std::ios::out);
		if (file.is_open())
		{
			file << isInstallationComplete;
			file.close();
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

	installEssentialTools(isInstallationComplete);

	file.open((home + std::string("/.aura/.cconfig")).c_str(), std::ios::out);
	if (file.is_open())
	{
		Log::log("writing to config file!", Type::E_DISPLAY);
		file << isInstallationComplete;
		file.close();
		std::cout << "done!\n";
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
void Aura::fixInstallation()
{
	Log::log("Are you sure you "
			 "want to "
			 "continue??[y/n]\n",
			 Type::E_DISPLAY);
	char c;
	while (true)
	{

		fscanf(stdin, "%c", &c);
		if (c != '\n')
			break;
	};

	if (tolower(c) != 'y')
		return;
#ifdef _WIN32
	std::string home = getenv(USERNAME);
#else
	std::string home{"/home/"};
	home += getenv(USERNAME);
#endif
	namespace fs = std::filesystem;
	Log::log("reseting aura...", Type::E_WARNING);
#ifdef _WIN32
	fs::remove_all((home + "\\.aura"));
#else
	fs::remove_all((home + "/.aura"));
#endif
	Log::log("all clean!", Type::E_DISPLAY);
	setup();
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
	system(("lldb ./build/debug/" + _project_setting.getProjectName() + ".exe").c_str());
#else
	system(("lldb ./build/debug/" + _project_setting.getProjectName()).c_str());
#endif
};
// TODO
// this is actually useless for now but will add usefull stuff to it in future
bool Aura::release()
{

	namespace fs = std::filesystem;
	std::string cpu_threads{std::to_string(std::thread::hardware_concurrency() - 1)};
	printf("%sThreads in use: %s%s\n", YELLOW, cpu_threads.c_str(), WHITE);
	if (!fs::exists(fs::current_path().string() + "/build/release"))
	{
		// run cmake
		Log::log("Compile Process has been started...", Type::E_DISPLAY);
		executeCMake(std::string("-Bbuild/release -DCMAKE_BUILD_TYPE=Release ") + _project_setting.getCMakeArgs()); // TODO
		// run ninja
		if (!system(("ninja -C build/release -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL", Type::E_DISPLAY);
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
		if (!system(("ninja -C build/release -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			Log::log("BUILD SUCCESSFULL", Type::E_DISPLAY);

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
	if (!fs::exists("config.json"))
		return;
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
	namespace fs = std::filesystem;
	try
	{
		fs::remove_all("build");
		executeCMake("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug " + _project_setting.getCMakeArgs());
		compile();
	}
	catch (std::exception &e)
	{
		Log::log(e.what(), Type::E_ERROR);
	};
};

void Aura::refresh()
{
	executeCMake("-Bbuild/debug -DCMAKE_BUILD_TYPE=Debug " + _project_setting.getCMakeArgs());
	if (fs::exists("build/release"))
		executeCMake("-Bbuild/release -DCMAKE_BUILD_TYPE=Release " + _project_setting.getCMakeArgs());
};
void Aura::buildDeps()
{
	// building cmake external libraries
	Deps deps;
	deps.buildDeps();
}
void Aura::addDeps()
{
	if (_args.size() < 3)
	{
		Log::log("You have to provide github url! make sure it has CMakeList.txt", Type::E_ERROR);
		return;
	};
	Deps deps;
	if (deps.addDeps(_args.at(2)))
		deps.buildDeps();
	else
		Log::log("Failed to add " + _args.at(2) + "make sure you solve those errors or remove it from external directory!", Type::E_ERROR);
};