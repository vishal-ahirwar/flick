#include <chrono>
#include <constants/constant.hpp>
#include <deps/deps.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <log/log.h>
#include <processmanager/processmanager.h>
#include <thread>
#include <unittester/unittester.h>
#include <vector>

namespace fs = std::filesystem;
UnitTester::UnitTester(const UserInfo& userInfo) { mUserInfo = userInfo; }
bool UnitTester::runUnitTesting(const std::vector<std::string>& args)
{

	namespace fs = std::filesystem;
	std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
	// auto formatedString = std::format("Threads in use : {}", cpuThreads.c_str());
	// Log::log(formatedString, Type::E_DISPLAY);
	if (!fs::exists(fs::current_path().string() + "/build/" + VCPKG_TRIPLET)) {
		// run cmake
		std::string processLog{};
		std::vector<std::string> args{"cmake", ".", "-DENABLE_TESTS=ON", "--preset=" + std::string(VCPKG_TRIPLET)};
		if (ProcessManager::startProcess(args, processLog, "Generating Tests CMake Files") != 0)
			return false;
		// run ninja
		args.clear();
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/" + VCPKG_TRIPLET);
		args.push_back("-j" + cpuThreads);
		return ProcessManager::startProcess(args, processLog, "Compiling Tests") ==
		       0; // if there is any kind of error then don't clear the terminal
	} else {
		std::string processLog{};
		std::vector<std::string> args{};
		args.push_back("cmake");
		args.push_back("--build");
		args.push_back("build/" + VCPKG_TRIPLET);
		args.push_back("-j" + cpuThreads);
		return ProcessManager::startProcess(args, processLog, "Compiling Tests") ==
		       0; // if there is any kind of error then don't clear the terminal
	}
};
//

/*
Make sure to call rebuild after calling this method
*/
void UnitTester::setupUnitTestingFramework()
{

	namespace fs = std::filesystem;
	auto path = fs::current_path().string() + "/tests/";
	if (fs::exists(path)) {
		return;
	};
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
	else if (input == "q") {
		std::exit(0);
	}
	fs::create_directories(path + "src");
	Deps deps;
	std::ofstream cmake{"./tests/CMakeLists.txt"};
	switch (lang) {
	case Language::C:

		[&]() -> void {
			std::fstream testFile(path + "src/main.c", std::ios::out);
			cmake << "find_package(cmocka CONFIG REQUIRED)\n";
			cmake << "add_executable(tests src/main.c)# Add your Source Files here\n";
			cmake << "target_link_libraries(tests PRIVATE cmocka::cmocka)\n";
			if (testFile.is_open()) {
				testFile << UNIT_TEST_CODE[static_cast<int>(lang)];
				testFile.close();
				deps.addDeps("cmocka", "");
			};
		}();
		break;
	case Language::CXX:
		[&]() -> void {
			std::fstream testFile(path + "src/main.cpp", std::ios::out);
			cmake << "find_package(GTest CONFIG REQUIRED)\n";
			cmake << "add_executable(tests src/main.cpp)# Add your Source Files here\n";
			cmake << "target_link_libraries(tests GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)\n";
			if (testFile.is_open()) {
				testFile << UNIT_TEST_CODE[static_cast<int>(lang)];
				testFile.close();
				deps.addDeps("gtest", "");
			};
		}();
		break;
	}
	cmake.close();
	cmake.open("CMakeLists.txt", std::ios::app);
	cmake << "add_subdirectory(tests)\n";
	cmake.close();
};