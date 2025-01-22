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
void addToConanFile(const std::string &);
void addToCMakeFile(std::string);

App::App() {

};
App::~App() {

};

void App::setupUnitTestingFramework()
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
	addToConanFile("catch2/3.7.1");
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
		fprintf(stderr, "%sFailed to open CMakeLists.txt%s", RED, WHITE);
		return;
	};
	file.close();
	reloadPackages();
	fprintf(stdout, "%sunit testing template code added to project run tests with : aura utest%s\n", YELLOW, WHITE);
};

void App::createNewProject(const char *argv[], int argc)
{

	printf("%sCreating directory ....\n%s", GREEN, WHITE);

	std::string cmdString{};
	createDir(argv[2]);
	printf("%sGenerating Code for main.c and CMakeLists.txt ....\n%s", GREEN, WHITE);
	generateConanFile();
	generateCppTemplateFile(argv[2]);
	generateCmakeFile(argv[2]);
	generateauraFile(_project_setting._project_name);
	generateGitIgnoreFile();
};

// TODO : add compile option
bool App::compile(const std::string &additional_cmake_arg)
{
	// Temp Soln

	namespace fs = std::filesystem;
	std::string cpu_threads{std::to_string(std::thread::hardware_concurrency())};
	printf("%sThreads in use: %s%s\n", YELLOW, cpu_threads.c_str(), WHITE);
	if (!fs::exists(fs::current_path().string() + "/build") || additional_cmake_arg.length() > 5)
	{
		// run cmake
		printf("%sCompile Process has been started ....\n%s", BLUE, WHITE);
		reloadPackages();
		// run ninja
		if (!system(("ninja -C build/Debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			printf("\n%sBUILD SUCCESSFULL%s\n", GREEN, WHITE);
			return true;
		}
		else
		{
			printf("\n%sBUILD FAILED%s\n", RED, WHITE);
			return false;
		}
	}
	else
	{
		// run ninja
		if (!system(("ninja -C build/Debug -j" + cpu_threads).c_str())) // if there is any kind of error then don't clear the terminal
		{
			printf("\n%sBUILD SUCCESSFULL%s\n", GREEN, WHITE);
			return true;
		}
		else
		{
			printf("\n%sBUILD FAILED%s\n", RED, WHITE);
			return false;
		}
	}
};
//
void App::run(int argc, const char **argv)
{
	std::string output{};
	readauraFile(output);
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
		printf("%s\n[error] Maybe You should Compile First Before run or You have Permission to "
			   "execute program!\n%s",
			   RED,
			   WHITE);
	};
}

//
void App::build()
{
	this->compile();
	this->run(0, nullptr);
}

//
void App::addToPathWin()
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
			std::cout << "aura doesn't exist in current dir\n";
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
				printf("%serror while copying aura.exe into aura directory!%s\n", RED, WHITE);
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
		std::cout << "All paths from aura are in PATH\n";
	}
	else
	{
		std::cout << "Some paths from aura are missing in PATH adding these entries into path make sure to restart your shell after that\n";
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
void App::addToPathUnix()
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
			std::cout << "aura doesn't exist in current dir\n";
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
				printf("%serror while copying aura.exe into aura directory!%s\n", RED, WHITE);
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
		std::cout << "All paths from aura are in PATH\n";
	}
	else
	{
		std::cout << "Some paths from aura are missing in PATH adding these entries into path make sure to restart your shell after that\n";
		pathStream.clear();
		pathStream.str(newPath);
		std::string tempStr{};
		std::string bashrc = std::string("/home/") + getenv(USERNAME) + "/.bashrc";
		std::fstream file(bashrc.c_str(), std::ios::app);
		if (file.is_open())
		{
			file << "export PATH=$PATH:" << newPath << "\n";
			file.close();
		}
		else
		{
			std::cout << "failed to open ~/.bashrc file!\n";
			return;
		}
	};
};

// installing dev tools
void App::installEssentialTools(bool &isInstallationComplete)
{
#ifdef WIN32
	namespace fs = std::filesystem;
	printf("%sThis will install C/C++ GCC Toolchain with cmake, ninja and conan package manager from Github,\nAre you sure you "
		   "want to "
		   "continue??[y/n] %s\n",
		   YELLOW,
		   WHITE);
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
	Downloader::download(std::string(CONAN_URL_64BIT), home + "\\conan.zip");
	Downloader::download(std::string(NINJA_URL_64BIT), home + "\\ninja.zip");
	Downloader::download(std::string(NSIS_URL), home + "\\nsis.zip");
	printf("%sunzipping file at %s%s\n", BLUE, home.c_str(), WHITE);
	if (system((std::string("tar -xf ") + "\"" + home + "\\compiler.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\cmake.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\conan.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\ninja.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	if (system((std::string("tar -xf ") + "\"" + home + "\\nsis.zip\"" + " -C " + "\"" + home + "\"").c_str()))
		return;
	printf("%sremoving downloaded archives...%s\n", RED, WHITE);
	fs::remove((home + "\\compiler.zip"));
	fs::remove((home + "\\cmake.zip"));
	fs::remove((home + "\\conan.zip"));
	fs::remove((home + "\\ninja.zip"));
	fs::remove((home + "\\nsis.zip"));
	isInstallationComplete = true;
	addToPathWin();
#else
#define DISTRO_INFO "/etc/os-release"
	// Todo :read the file and u will know what ditro user running ;)
	try
	{
		namespace fs = std::filesystem;
		std::string aura{"/home/"};
		aura += {getenv(USERNAME)};
		aura += "/.aura";
		Downloader::download(std::string(CONAN_LINUX_URL), aura + "/conan.tgz");
		system(("tar -xvf " + aura + "/conan.tgz" + " -C " + aura).c_str());
		system(("chmod +x " + aura + "/bin/conan").c_str());
		addToPathUnix();
		std::ifstream file(DISTRO_INFO);
		if (!file.is_open())
			return;
		std::string distro_name{};
		while (std::getline(file, distro_name))
		{
			if (distro_name.find("ID_LIKE") != std::string::npos)
				break;
		};
		if (distro_name.find("ID_LIKE") == std::string::npos)
		{
			file.clear();
			file.seekg(0, file.beg);
			while (std::getline(file, distro_name))
			{
				if (distro_name.length() > 0 && distro_name.substr(0, 2) == "ID")
					break;
			};
		}
		auto index = distro_name.find("=");
		if (index == std::string::npos)
			return;
		distro_name = distro_name.substr(++index, distro_name.length());
		std::cout << GREEN << "Development OS Distro/Parent Distro : " << distro_name << WHITE << "\n";
		if (distro_name.find("debian") != std::string::npos || distro_name.find("ubuntu") != std::string::npos)
		{
			system("sudo apt install g++ cmake git");
		}
		else if (distro_name.find("arch") != std::string::npos)
		{
			system("pacman -Sy g++ cmake git");
		}
		else if (distro_name.find("fedora") != std::string::npos || distro_name.find("rhel") != std::string::npos)
		{
			system("sudo dnf install g++ cmake git");
		};

		file.close();

		isInstallationComplete = true;
	}
	catch (std::exception &e)
	{
		printf("%sError : %s\nmake sure you have root privileges  to run this command%s\n", RED, e.what(), WHITE);
	};

#endif
};
//
void App::setup()
{
	onSetup();
};

// TODO
void App::generateauraFile(const std::string &path)
{
	std::string newFileName{path};
	newFileName += "/setting.nn";
	std::ofstream file(newFileName.c_str(), std::ios::out);
	if (file.is_open())
	{
		time_t now{time(NULL)};
		const char *dateTime{ctime(&now)};

		file << _project_setting._project_name << "\n";
		file << "Project created on " << dateTime << "\n";
		printf("%sProject Creation date : %s%s", YELLOW, dateTime, WHITE);
		file << "Note: Please don't remove or edit this file!\n";
		file.close();
	}
	else
	{
		printf("%ssomething went wrong!\n%s", RED, WHITE);
	}
}

// TODO
// reading project configuration file
void App::readauraFile(std::string &output)
{
	std::ifstream file("setting.nn");
	if (file.is_open())
	{
		std::getline(file, output);
		std::string dateTime{};
		std::getline(file, dateTime);
		printf("%s[%s: %s]%s\n\n", YELLOW, output.c_str(), dateTime.c_str(), WHITE);
		file.close();
	}
	else
	{
		printf("%saura file setting.nn doesn't exist!\n%s", RED, WHITE);
	};
}

// creating folder structure for project
void App::createDir(const char *argv)
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
		printf("%sfailed to create dir error!%s", RED, WHITE);
		exit(0);
	}
};
//
void App::generateCppTemplateFile(const char *argv)
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

	std::ofstream setup_file("./" + _project_setting._project_name + "/setup.py");
	if (setup_file.is_open())
	{
		setup_file << "print('No External Library configuration added yet!')";
	};
	setup_file.close();
};

//
void App::generateCmakeFile(const char *argv)
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
void App::generateGitIgnoreFile()
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
void App::generateLicenceFile()
{
	std::ofstream out;
	out.open("License.txt", std::ios_base::out);
	if (!out.is_open())
	{
		printf("%s[Error]Failed to Generate License.txt, You may need to create License.txt by "
			   "yourself :)%s",
			   RED,
			   WHITE);
		return;
	};
	out << LICENSE_TEXT;
	out.close();
}
//
void App::generateConanFile()
{
	std::ofstream file;
	file.open("./" + _project_setting._project_name + "/conanfile.txt", std::ios::out);
	if (file.is_open())
	{
		file << CONAN_CODE;
		file.close();
	}
	else
	{
		printf("%s[error]Failed to generate conanfile.txt%s\n", RED, WHITE);
	};
};
// creating packaged build [with installer for windows] using cpack
void App::createInstaller()
{
	if (!release())
	{
		fprintf(stderr, "%s[Error] : Please First fix all the errors%s", RED, WHITE);
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
		reloadPackages();
		if (system("cd build/Release && cpack"))
			printf("%s[Msg]CPack added to cmake run 'aura createinstaller' command again"
				   "%s\n",
				   GREEN,
				   WHITE);
	}
	else
	{
		printf("%s[Msg]Something went wrong :(%s\n", RED, WHITE);
	}
};

// running utest
void App::test()
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
bool App::onSetup()
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
		printf("%saura dir alread exist%s\n", GREEN, WHITE);
	};
	file.open((home + "\\.aura\\.cconfig").c_str(), std::ios::in);
	if (file.is_open())
	{
		file >> isInstallationComplete;
		file.close();
		if (isInstallationComplete)
		{
			printf("%sCompiler is already installed if you think you messed up with aura installation please use this command aura fix %s\n", GREEN, WHITE);
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
		printf("%saura dir alread exist%s\n", GREEN, WHITE);
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
			printf("%sCompiler is already installed if you think you messed up with aura installation please use this command aura fix %s\n", GREEN, WHITE);
			return true;
		}
	}
	else
	{
		std::cout << "config file doesn't exist, creating one\n";
	};

	installEssentialTools(isInstallationComplete);

	file.open((home + std::string("/.aura/.cconfig")).c_str(), std::ios::out);
	if (file.is_open())
	{
		std::cout << "writing to config file!\n";
		file << isInstallationComplete;
		file.close();
		std::cout << "done!\n";
		return true;
	}
	else
	{
		std::cout << "failed to write config file\n";
		return false;
	};

#endif
}
// TODO
// remove the ~/aura and reinstall the aura again with all the tools like cmake,g++ compiler,ninja,nsis
void App::fixInstallation()
{
	printf("%sAre you sure you "
		   "want to "
		   "continue??[y/n] %s\n",
		   YELLOW,
		   WHITE);
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
	printf("%sreseting aura...%s\n", RED, WHITE);
#ifdef WIN32
	fs::remove_all((home + "\\.aura"));
#else
	fs::remove_all((home + "/.aura"));
#endif
	printf("%sall clean!%s\n", RED, WHITE);
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
		printf("%sUpdater started successfully.%s\n", GREEN, WHITE);
	}
	else
	{
		printf("%sFailed to start updater!\n", RED, WHITE);
		if (GetLastError() == 740)
		{
			printf("%sPlease try run this command with administrator privileges%s\n", RED, WHITE);
		}
		else
		{
			printf("%sunkown error occured!%s\n", RED, WHITE);
		};
	}
#else
	printf("%sImpementation is still in development for linux%s\n", BLUE, WHITE);
	// Linux implementation using fork() and exec()
	pid_t pid = fork(); // Create a new process

	if (pid < 0)
	{
		std::cerr << "Fork failed!" << std::endl;
		return;
	}

	if (pid == 0)
	{
		// This is the child process
		// Execute the update tool
		if (execlp(path.c_str(), path.c_str(), (char *)NULL) == -1)
		{
			printf("Failed to start update tool\n");
			return;
		};
	}
	else
	{
		// Parent process - can optionally wait for the child to finish or log the success
		std::cout << "Updater started successfully in the background." << std::endl;
		return;
	}
#endif
}
// download the utool from github if it's not already there then download the latest build from github
void App::update()
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
	printf("updating aura...\n");
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
void App::debug()
{
	readauraFile(_project_setting._project_name);
	system(("gdb ./build/Debug/" + _project_setting._project_name).c_str());
};
// TODO
// this is actually useless for now but will add usefull stuff to it in future
bool App::release()
{
	if (system("conan install . --build=missing --settings=build_type=Release"))
		return false;
	if (system("cmake --preset conan-release -G \"Ninja\""))
		return false;
	if (system("ninja -C ./build/Release"))
		return false;
	return true;
};

// writing to conanfile.txt without checking if the package is already in conanfile.txt, for that checks are in add() method
void addToConanFile(const std::string &name)
{
	std::ifstream in("conanfile.txt");
	if (!in.is_open()) // checking if conanfile.txt presents or not
	{
		fprintf(stderr, "%s[error]Failed to open conanfile.txt, try aura initconan%s\n", RED, WHITE);
		return;
	};
	std::vector<std::string> lines{};
	std::string line{};
	while (std::getline(in, line)) // reading whole conanfile.txt line by line in vector so we can easily update the conanfile.txt at a particular line
	{
		lines.push_back(line);
	};
	in.close();
	lines.insert(lines.begin() + 1, name);
	std::ofstream out("conanfile.txt");
	if (!out.is_open())
		return;
	for (const auto &l : lines) // writing back to file the updated contents
	{
		out << l << "\n";
	};
	out.close();
}

void addToCMakeFile(std::string name)
{
	auto index = name.find("/");
	name = name.substr(0, index); // extracting package name from conan format string like fmt/1.2.0 there the package name is fmt

	std::transform(name.begin(), name.end(), name.begin(), ::tolower); // this may not work for all packages downloaded from conan
	std::ifstream in("CMakeLists.txt");
	if (!in.is_open())
	{
		printf("%s[error]Failed to open CMakeLists.txt%s\n", RED, WHITE);
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
void App::addConanPackage(const std::string &name)
{
	std::ifstream file{"conanfile.txt"};
	if (!file.is_open())
	{
		fprintf(stderr, "%s[Error] : failed to open conanfile.txt%s", RED, WHITE);
		return;
	};
	std::string line{};
	while (std::getline(file, line))
	{
		if (line.find(name) != std::string::npos) // if name is already in conanfile.txt don't add it again ;)
		{
			fprintf(stderr, "%s%s is already in conanfile.txt%s\n", YELLOW, name.c_str(), WHITE);
			return;
		}
	};
	file.close();
	addToConanFile(name); // add to conanfile then install if install fails don't add it to cmakelists.txt
	if (system("conan install . --build=missing --settings=build_type=Debug"))
	{
		fprintf(stderr, "%s[Error] : Failed to install %s package\n%s", RED, name.c_str(), WHITE);
		return;
	}
	printf("%s[Msg] : %s added to conanfile.txt and installed successfully%s\n", GREEN, name.c_str(), WHITE);
	addToCMakeFile(name);
	reloadPackages();
};

// installing newly added packages or reloading CMake configuration
void App::reloadPackages()
{
	if (system("conan install . --build=missing --settings=build_type=Debug"))
		return;
	if (system("cmake --preset conan-debug -G \"Ninja\""))
		return;
}

// for generating conan file for any CMAKE project
void App::initConan()
{
	std::ifstream conan_file{"conanfile.txt"};
	if (conan_file.is_open())
	{
		fprintf(stderr, "%sconanfile.txt already exist! you should try aura reload%s\n", RED, WHITE);
		conan_file.close();
		return;
	};
	generateConanFile();
	reloadPackages();
}

// for generating vscode intelligence
// everytime user run this command it's will override everything in c_cpp_properties.json
void App::vsCode()
{
	namespace fs = std::filesystem;
	if (!fs::exists("setting.nn"))
		return;
	fs::exists(".vscode") ? fprintf(stdout, "%s.vscode already exist!%s\n", YELLOW, WHITE) : fs::create_directory(".vscode");
	std::ofstream file(".vscode/c_cpp_properties.json", std::ios::out);
	if (file.is_open())
	{
		file << VSCODE_CONFIG;
	}
	else
	{
		fprintf(stderr, "%sfailed to create .vscode/c_cpp_properties.json%s\n", RED, WHITE);
	}
	file.close();
}

// it will simply delete the whole build folder and compile the project again
void App::reBuild()
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

void App::askUserinfo(struct UserInfo *user) {

};

void App::readUserInfoFromConfigFile(struct UserInfo *user) {

};

void App::writeUserInfoToConfigFile(struct UserInfo *user) {

};

void App::readProjectSettings(struct ProjectSetting *setting) {

};
void App::writeProjectSettings(struct ProjectSetting *setting) {

};
bool App::checkIfConanNeeded(void)
{
	return false;
};
