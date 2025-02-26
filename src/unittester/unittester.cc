#include <unittester/unittester.h>
#include <fstream>
#include <log/log.h>
#include <constants/constant.hpp>
#include <format>
#include <chrono>
UnitTester::UnitTester(const UserInfo &user_info)
{
    _user_info = user_info;
}
void UnitTester::runUnitTesting() {

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
    Log::log("unit testing template code added to project run tests with : UnitTester utest", Type::E_DISPLAY);
};