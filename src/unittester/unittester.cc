#include <unittester/unittester.h>
#include <fstream>
#include <log/log.h>
#include <constants/constant.hpp>
#include <format>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <deps/deps.h>
#include <vector>
#include <thread>

namespace fs = std::filesystem;
UnitTester::UnitTester(const UserInfo &userInfo)
{
    mUserInfo = userInfo;
}
void UnitTester::runUnitTesting(const std::vector<std::string> &args)
{

    namespace fs = std::filesystem;
    std::string cpuThreads{std::to_string(std::thread::hardware_concurrency() - 1)};
    auto formatedString = std::format("Threads in use : {}", cpuThreads.c_str());
    Log::log(formatedString, Type::E_DISPLAY);
    if (!fs::exists(fs::current_path().string() + "/build/tests"))
    {
        for (auto &arg : args)
        {
            if (arg.find("--nostatic") != std::string::npos)
            {
                VCPKG_TRIPLET = "default";
                break;
            }
        };
        // run cmake
        Log::log("Compile Process has been started...", Type::E_DISPLAY);
        system((std::string("cmake . -Bbuild/tests -DENABLE_TESTS=ON --preset=") + std::string(VCPKG_TRIPLET)).c_str()); // TODO
        // run ninja
        system(("cmake --build build/tests -j" + cpuThreads).c_str()); // if there is any kind of error then don't clear the terminal
    }
    else
    {
        // run ninja
        system(("cmake --build build/tests -j" + cpuThreads).c_str()); // if there is any kind of error then don't clear the terminal
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
    if (fs::exists(path))
    {
        return;
    };
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
        std::exit(0);
    }
    fs::create_directory(path);
    Deps deps;
    switch (lang)
    {
    case Language::C:

        [&]() -> void
        {std::fstream testFile(path + "main.c", std::ios::out);
        if (testFile.is_open())
        {
            testFile << UNIT_TEST_CODE[static_cast<int>(lang)];
            testFile.close();
            deps.addDeps("cmocka");
        }; }();
        break;
    case Language::CXX:
        [&]() -> void
        {std::fstream testFile(path + "main.cc", std::ios::out);
    if (testFile.is_open())
    {
        testFile << UNIT_TEST_CODE[static_cast<int>(lang)];
        testFile.close();
        deps.addDeps("gtest");
    }; }();
        break;
    }
};