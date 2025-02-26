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
void UnitTester::generateLicenceFile()
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
    std::string _licence{LICENSE_TEXT};
    // TODO
    constexpr std::string_view year{"@_YEAR_"};
    constexpr std::string_view name{"@_OWNER_"};
    auto index = _licence.find(year);
    if (index != std::string::npos)
        _licence.replace(index, year.length(), std::format("{:%Y}", std::chrono::system_clock::now()));
    index = _licence.find(name);
    if (index != std::string::npos)
        _licence.replace(index, name.length(), _user_info.getUserName());
    out << _licence;
    out.close();
}
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