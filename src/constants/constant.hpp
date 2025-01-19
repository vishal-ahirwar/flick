#ifndef _CONSTANT_
#define _CONSTANT_
#include <string>
constexpr std::string_view LICENSE_TEXT{R"(
Copyright(C)<YEAR> <COPYRIGHT_HOLDER>.

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE)"};

constexpr std::string_view CPACK_CODE{R"(
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR ${COMPANY})
include(CPack))"};

constexpr std::string_view GITIGNORE_CODE{
    R"(
CMakeLists.txt.user
CMakeCache.txt
CMakeFiles
CMakeScripts
Testing
Makefile
cmake_install.cmake
install_manifest.txt
compile_commands.json
CTestTestfile.cmake
_deps
build
)"};

constexpr std::string_view CMAKE_CODE{
    R"(
#Auto Genrated CMake file by aura CLI
#Copyright 2023 Vishal Ahirwar. #replace with your copyright notice.
cmake_minimum_required(VERSION 3.6...3.31)
project(@ VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops")
set(COMPANY "@DeveloperName")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} ${COMPANY}.")
include_directories(src ${CMAKE_BINARY_DIR})
configure_file(@config_in @config_h)
#@find Warning: Do not remove this line
set(SOURCE ./src/main.cc)#add your additional source file here!
add_executable(${PROJECT_NAME} ${SOURCE})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#@link Warning: Do not remove this line
)"};


std::string MAIN_CODE{
    R"(
//Auto Genrated C++ file by aura CLI
//Copyright 2023 Vishal Ahirwar //replace it with your copyright notice!
#include<cstdio>
_HEADER_
int main(int argc,char*argv[])
{
    fprintf(stderr,"Project Name : %s, %s\n",_PROJECT_,_COPYRIGHT_);//This is Template Code
    return 0;
};

)"};

constexpr std::string_view TEST_CXX_CODE{R"(
#include <catch2/catch_test_macros.hpp>

unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(5) == 120);
})"};
constexpr std::string_view CONAN_CODE{R"([requires]

[generators]
CMakeDeps
CMakeToolchain
[layout]
cmake_layout
)"};

constexpr std::string_view VSCODE_CONFIG{R"(   {
       "configurations": [
           {
               "name": "Win32",
               "compileCommands": "${workspaceFolder}/build/Debug/compile_commands.json",
               "includePath": [
                   "${workspaceFolder}/**"
               ],
               "defines": [
                   "_DEBUG",
                   "UNICODE",
                   "_UNICODE"
               ],
               "windowsSdkVersion": "10.0.22621.0",
               "cStandard": "c17",
               "cppStandard": "c++17",
               "intelliSenseMode": "windows-gcc-x64"
           }
       ],
       "version": 4
   })"};
constexpr std::string_view VIM_CONFIG{R"()"};

#ifdef WIN32
constexpr std::string_view COMPILER_URL_64BIT{"https://github.com/cristianadam/mingw-builds/releases/download/v12.1.0-rev0/x86_64-12.1.0-release-posix-seh-rt_v10-rev0.7z"};

constexpr std::string_view CMAKE_URL_64BIT{"https://github.com/Kitware/CMake/releases/download/v3.31.2/cmake-3.31.2-windows-x86_64.zip"};

constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/utool.exe"};

constexpr std::string_view CONAN_URL_64BIT{"https://github.com/conan-io/conan/releases/download/2.11.0/conan-2.11.0-windows-x86_64.zip"};

constexpr std::string_view NINJA_URL_64BIT{"https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"};

constexpr std::string_view NSIS_URL{"https://cyfuture.dl.sourceforge.net/project/nsis/NSIS%203/3.10/nsis-3.10.zip?viasf=1"};//TODO

#else
constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/utool"
};
constexpr std::string_view CONAN_LINUX_URL{"https://github.com/conan-io/conan/releases/download/2.11.0/conan-2.11.0-linux-x86_64.tgz"};
#endif
#endif
