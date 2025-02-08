#include <aura/aura.hpp>
#include <constants/colors.hpp>
#include <thread>
#include <constants/constant.hpp>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif
#include <downloader/downloader.h>

#ifdef WIN32
#define USERNAME "USERPROFILE"
#else
#define USERNAME "USER"
#endif

#include <vector>
#include <algorithm>
#include <rt/rt.h>
#include <log/log.h>

Aura::Aura() {

};
Aura::~Aura() {

};

void Aura::setupUnitTestingFramework()
{
	namespace fs = std::filesystem;
	auto path = fs::current_path().string() + "/tests/";
	if (fs::exists(path))
	{
		return;
	};
	fs::create_directory(path);

	std::fstream testFile(path + "main.cc", std::ios::out);
	if (testFile.is_open())
	{
		testFile << TEST_CXX_CODE;
		testFile.close();
	};
	std::ofstream file{"./CMakeLists.txt", std::ios::app};
	if (file.is_open())
	{
		file << "#Unit Testing CMake Section\n";
		file << "find_package(Catch2)\n";
		file << "add_executable(tests ./tests/main.cc)\n";
		file << "target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)\n";
	}
	else
	{
		Log::log("Failed to open CMakeLists.txt", Type::E_ERROR);
		return;
	};
	file.close();
	reBuild();
	Log::log("unit testing template code added to project run tests with : aura utest", Type::E_DISPLAY);
};

void Aura::createNewProject(const char *argv[], int argc)
{

	Log::log("Creating directory..", Type::E_DISPLAY);
	std::string cmdString{};
	createDir(argv[2]);
	Log::log("Generating Code for main.c and CMakeLists.txt..", Type::E_DISPLAY);
	generateCppTemplateFile(argv[2]);
	generateCmakeFile(argv[2]);
	generateBuildFile(_project_setting._project_name);
	generateGitIgnoreFile();
	Log::log("Done", Type::E_DISPLAY);
};

// TODO : add compile option
bool Aura::compile(const std::string &additional_cmake_arg)
{
	// Temp Soln

	namespace fs = std::filesystem;
	std::string cpu_threads{std::to_string(std::thread::hardware_concurrency())};
	printf("%sThreads in use: %s%s\n", YELLOW, cpu_threads.c_str(), WHITE);
	if (!fs::exists(fs::current_path().string() + "/build") || additional_cmake_arg.length() > 5)
	{
		// run cmake
		Log::log("Compile Process has been started...", Type::E_DISPLAY);
		// run ninja
		if (!system(("ninja -C build/Debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
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
		if (!system(("ninja -C build/Debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
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
void Aura::run(int argc, const char **argv)
{
	std::string output{};
	readBuildFile(output);
	_project_setting._project_name = output;

	std::string run{};
	// printf("%s%s: \n%s", YELLOW, projectName.c_str(),WHITE);
#ifdef WIN32
	run += ".\\build\\Debug\\";
	run += _project_setting._project_name;
	run += ".exe";
#else
	run += "./build/Debug/";
	run += _project_setting._project_name;
#endif // WIN32
	for (int i = 0; i < argc; ++i)
	{
		run += " ";
		run += argv[i];
	};

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
	this->compile();
	this->run(0, nullptr);
}

//
void Aura::addToPathWin()
{
#ifdef WIN32
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
	aura += {getenv(USERNAME)};
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
#ifdef WIN32
	namespace fs = std::filesystem;
	Log::log("This will install C/C++ GCC Toolchain with cmake, ninja and conan package manager from Github,\nAre you sure you "
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

// TODO
void Aura::generateBuildFile(const std::string &path)
{
	std::string newFileName{path};
	newFileName += "/build.py";
	std::ofstream file(newFileName.c_str(), std::ios::out);
	if (file.is_open())
	{
		constexpr std::string_view project_name{"@projectName"};
		constexpr std::string_view build_date{"@builddatetime"};
		time_t now{time(NULL)};
		char *dateTime{ctime(&now)};
		dateTime[strlen(dateTime) - 1] = '\0';
		std::string build_py(BUILD_PY);
		auto index = build_py.find(project_name);
		if (index == std::string::npos)
			return;
		build_py.replace(index, project_name.length(), _project_setting._project_name);
		index = build_py.find(build_date);
		if (index == std::string::npos)
			return;
		build_py.replace(index, build_date.length(), dateTime);
		file << build_py;
		file.close();
	}
	else
	{
		Log::log("something went wrong!", Type::E_ERROR);
	}
}

// TODO
// reading project configuration file
void Aura::readBuildFile(std::string &output)
{
	std::ifstream file("build.py");
	if (file.is_open())
	{
		std::getline(file, output);
		std::getline(file, output);
		std::string dateTime{};
		std::getline(file, dateTime);
		auto index = output.find("=");
		if (index == std::string::npos)
		{
			Log::log("build.py seems incorrect!", Type::E_ERROR);
			return;
		};
		output = output.substr(index + 1);
		index = dateTime.find("=");
		if (index == std::string::npos)
		{
			Log::log("build.py seems incorrect!", Type::E_ERROR);
			return;
		};
		dateTime = dateTime.substr(index + 1);
		printf("%s[%s: %s]%s\n\n", YELLOW, output.c_str(), dateTime.c_str(), WHITE);
		file.close();
	}
	else
	{
		Log::log("build.py doesn't exist!", Type::E_ERROR);
	};
}

// creating folder structure for project
void Aura::createDir(const char *argv)
{
	namespace fs = std::filesystem;
	std::string cmdString{};
	_project_setting._project_name = argv;
	cmdString += argv;
	if (fs::create_directory(cmdString.c_str()))
	{
		cmdString += "/src";
		fs::create_directory(cmdString.c_str());
		auto pos = cmdString.find("/");
		cmdString.replace(pos + 1, cmdString.length() - pos, "res");
		fs::create_directory(cmdString.c_str());
		pos = cmdString.find("/");
		cmdString.replace(pos + 1, cmdString.length() - pos, "external");
		fs::create_directory(cmdString.c_str());
		std::ofstream file(cmdString + "/help.txt");
		if (file.is_open())
		{
			file << "Add your external libraries in this directory, if librarary is not available on conan";
		};
		file.close();
	}
	else
	{
		Log::log("failed to create dir error!", Type::E_ERROR);
		exit(0);
	}
};
//
void Aura::generateCppTemplateFile(const char *argv)
{
	std::ofstream file;
	// const std::string stdStr{argv[2]};
	_project_setting._project_name = argv;
	file.open("./" + _project_setting._project_name + "/src/main.cc", std::ios::out);

	if (file.is_open())
	{
		std::string header{"_HEADER_"};
		std::string copyright{"_COPYRIGHT_"};
		std::string project{"_PROJECT_"};
		std::string cap;
		cap.resize(_project_setting._project_name.length());
		std::transform(_project_setting._project_name.begin(), _project_setting._project_name.end(), cap.begin(), ::toupper);
		auto index = MAIN_CODE.find(header);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, header.length(), ("#include<" + _project_setting._project_name + "config.h>"));
		index = MAIN_CODE.find(copyright);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, copyright.length(), (cap + "_COPYRIGHT"));
		index = MAIN_CODE.find(project);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, project.length(), "\"" + _project_setting._project_name + "\"");
		file << MAIN_CODE;
		file.close();
	};
};

//
void Aura::generateCmakeFile(const char *argv)
{
	std::string config{(_project_setting._project_name + "config.h.in")};
	{
		std::ofstream file;
		std::string cap;
		cap.resize(_project_setting._project_name.length());
		std::transform(_project_setting._project_name.begin(), _project_setting._project_name.end(), cap.begin(), ::toupper);
		file.open("./" + _project_setting._project_name + "/" + config, std::ios::out);
		if (file.is_open())
		{
			file << ("#define " + cap + "_VERSION_MAJOR @" + _project_setting._project_name + "_VERSION_MAJOR@") << std::endl;
			file << ("#define " + cap + "_VERSION_MINOR @" + _project_setting._project_name + "_VERSION_MINOR@") << std::endl;
			file << ("#define " + cap + "_VERSION_PATCH @" + _project_setting._project_name + "_VERSION_PATCH@") << std::endl;
			file << ("#define " + cap + "_COMPANY" + " \"@COMPANY@\"") << std::endl;
			file << ("#define " + cap + "_COPYRIGHT" + " \"@COPYRIGHT@\"") << std::endl;
			file.close();
		};
	}
	{

		std::ofstream file;
		file.open("./" + _project_setting._project_name + "/CMakeLists.txt", std::ios::out);
		std::string config_in{"@config_in"};
		std::string config_h{"@config_h"};
		if (file.is_open())
		{
			std::string str(CMAKE_CODE);
			auto index = str.find("@");
			if (index != std::string::npos)
			{
				str.replace(index, 1, _project_setting._project_name);
			};
			index = str.find(config_h);
			if (index != std::string::npos)
				str.replace(index, config_h.length(), (_project_setting._project_name + "config.h"));
			index = str.find(config_in);
			if (index != std::string::npos)
				str.replace(index, config_in.length(), config);
			file << str;
			file.close();
		};
	}
}
//
void Aura::generateGitIgnoreFile()
{
	std::ofstream file;
	file.open("./" + _project_setting._project_name + "/.gitignore", std::ios::out);
	if (file.is_open())
	{
		file << GITIGNORE_CODE;

		file.close();
	};
};
//
void Aura::generateLicenceFile()
{
	std::ofstream out;
	out.open("License.txt", std::ios_base::out);
	if (!out.is_open())
	{
		Log::log("Failed to Generate License.txt, You may need to create License.txt by "
				 "yourself :)%s",
				 Type::E_ERROR);
		return;
	};
	out << LICENSE_TEXT;
	out.close();
}
//

// creating packaged build [with installer for windows] using cpack
void Aura::createInstaller()
{
	if (!release())
	{
		Log::log("Please First fix all the errors", Type::E_ERROR);
		return;
	};
	if (!system("cd build/Release && cpack"))
		return;
	std::ofstream file;
	file.open("CMakeLists.txt", std::ios::app);
	if (file.is_open())
	{
		file << CPACK_CODE;
		file.close();
		generateLicenceFile();
		if (system("cd build/Release && cpack"))
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
	setupUnitTestingFramework();
	compile();
#ifdef WIN32
	system(".\\build\\Debug\\tests.exe");
#else
	system("./build/Debug/tests");
#endif
};

// TODO
// implementation is buggy right now will fix later
bool Aura::onSetup()
{
	bool isInstallationComplete{false};
	namespace fs = std::filesystem;
#ifdef WIN32
	std::string home = getenv(USERNAME);
#else
	std::string home{"/home/"};
	home += getenv(USERNAME);
#endif
	if (!home.c_str())
		return false;
	std::fstream file;
#ifdef WIN32
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
			 "continue??[y/n] %s\n",
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
#ifdef WIN32
	std::string home = getenv(USERNAME);
#else
	std::string home{"/home/"};
	home += getenv(USERNAME);
#endif
	namespace fs = std::filesystem;
	Log::log("reseting aura...", Type::E_WARNING);
#ifdef WIN32
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
#ifdef WIN32
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
#ifdef WIN32
	std::string aura = getenv(USERNAME);
#else
	std::string aura{"/home/"};
	aura += getenv(USERNAME);
#endif
#ifdef WIN32
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
#ifndef WIN32 // for linux we have to set permission for the newly downloaded file
		system(("chmod +x " + source).c_str());
#endif
		createProcess(source); // starting process parent-less which will start utool so aura will shutdown and then utool override the aura.exe
	};
};
// TODO
void Aura::debug()
{
	readBuildFile(_project_setting._project_name);
	if (!compile())
		return;
	system(("gdb ./build/Debug/" + _project_setting._project_name).c_str());
};
// TODO
// this is actually useless for now but will add usefull stuff to it in future
bool Aura::release()
{
	namespace fs = std::filesystem;
	if (fs::exists("conanfile.txt"))
	{

		if (system("conan install . --build=missing --settings=build_type=Release"))
			return false;
		if (system("cmake --preset conan-release -G \"Ninja\""))
			return false;
		if (system("ninja -C ./build/Release"))
			return false;
	}
	else
	{
		if (system("cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release -G \"Ninja\""))
			return false;
		if (system("ninja -C ./build/Release"))
			return false;
	}
	return true;
};

void addToCMakeFile(std::string name)
{
	auto index = name.find("/");
	name = name.substr(0, index); // extracting package name from conan format string like fmt/1.2.0 there the package name is fmt

	std::transform(name.begin(), name.end(), name.begin(), ::tolower); // this may not work for all packages downloaded from conan
	std::ifstream in("CMakeLists.txt");
	if (!in.is_open())
	{
		Log::log("Failed to open CMakeLists.txt", Type::E_ERROR);
		return;
	};
	std::vector<std::string> lines{};
	std::string line{};
	while (std::getline(in, line)) // reading whole file in vector to easily update the file
	{
		lines.push_back(line);
	};
	in.close();
	auto pos = lines.size() - 1;
	for (int i = 0; i < lines.size(); ++i)
	{
		if (lines[i].find("#@find") != std::string::npos)
		{
			pos = ++i; // add the find_package(name) after this line
			break;
		};
	};
	lines.insert(lines.begin() + pos, "find_package(" + name + " REQUIRED)"); // NOTE
	for (int i = 0; i < lines.size(); ++i)
	{
		if (lines[i].find("@link") != std::string::npos)
		{
			pos = ++i; // add the target_link_libraries(${PROJECT_NAME} after this line
			break;
		}
	};
	lines.insert(lines.begin() + pos, "target_link_libraries(${PROJECT_NAME} " + name + "::" + name + ")"); // NOTE
	std::ofstream out("CMakeLists.txt");
	if (!out.is_open())
		return;
	for (const auto &l : lines) // writing back to file the updated contents
	{
		out << l << "\n";
	};
	out.close();
};
// adding new packages to conanfile.txt then add recuired commands to  cmakelists.txt and reload cmakelist.txt to reconfigure project




// for generating vscode intelligence
// everytime user run this command it's will override everything in c_cpp_properties.json
void Aura::vsCode()
{
	namespace fs = std::filesystem;
	if (!fs::exists("build.py"))
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
		if (fs::exists("./build"))
		{
			fs::remove_all("./build");
		}
		/* code */
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return;
	}

	compile();
};

void Aura::askUserinfo(struct UserInfo *user) {

};

void Aura::readUserInfoFromConfigFile(struct UserInfo *user) {

};

void Aura::writeUserInfoToConfigFile(struct UserInfo *user) {

};

void Aura::readProjectSettings(struct ProjectSetting *setting) {

};
void Aura::writeProjectSettings(struct ProjectSetting *setting) {

};
bool Aura::checkIfConanNeeded(void)
{
	return false;
};
