#ifndef _CONSTANT_
#define _CONSTANT_
#include <string>
constexpr std::string_view LICENSE_TEXT{R"(
Copyright (c) @_YEAR_, @_OWNER_
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions, and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions, and the following disclaimer in the documentation or other materials provided with the distribution.
3. Neither the name of the [organization] nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
)"};

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
install
__pycache__
)"};

constexpr std::string_view CMAKE_CODE{
    R"(
#Auto Genrated CMake file by aura CLI
#@COPYRIGHT
cmake_minimum_required(VERSION 3.6...3.31)
project(@name VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fno-omit-frame-pointer -fno-inline")
set(COMPANY "@DeveloperName")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} ${COMPANY}.")
include_directories(src ${CMAKE_BINARY_DIR})
configure_file(@config_in @config_h)
#@find Warning: Do not remove this line
file(GLOB SOURCES "src/*.cxx")
add_executable(${PROJECT_NAME} ${SOURCES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#@link Warning: Do not remove this line
)"};

std::string MAIN_CODE{
    R"(
//Auto Genrated C++ file by aura CLI
//@COPYRIGHT
#include<iostream>
_HEADER_
int main(int argc,char*argv[])
{
    std::cerr<<"Hello, "<<_PROJECT_<<std::endl;
    std::cerr<<_COPYRIGHT_<<std::endl;
    if(argc<2)return 0;
    std::cerr<<"Args: ";
    for(int i=1;i<argc;++i)
    {
        std::cerr<<argv[i]<<" ";
    };
    std::cerr<<std::endl;
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


constexpr std::string_view VIM_CONFIG{R"()"};

#ifdef WIN32
constexpr std::string_view VSCODE_CONFIG{R"(    {
       "configurations": [
           {
               "name": "Windows OS",
               "compileCommands": "${workspaceFolder}/build/debug/compile_commands.json",
               "includePath": [
                   "${workspaceFolder}/**"
               ],
               "defines": [
                   "_DEBUG",
                   "UNICODE",
                   "_UNICODE"
               ],
               "windowsSdkVersion": "10.0.22621.0",
               "cStandard": "gnu23",
               "cppStandard": "c++20",
               "intelliSenseMode": "windows-clang-x86"
           }
       ],
       "version": 4
   })"};

constexpr std::string_view COMPILER_URL_64BIT{"https://github.com/mstorsjo/llvm-mingw/releases/download/20250114/llvm-mingw-20250114-ucrt-x86_64.zip"};
constexpr std::string_view CMAKE_URL_64BIT{"https://github.com/Kitware/CMake/releases/download/v3.31.2/cmake-3.31.2-windows-x86_64.zip"};
constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/utool.exe"};
constexpr std::string_view NINJA_URL_64BIT{"https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"};
constexpr std::string_view NSIS_URL{"https://cyfuture.dl.sourceforge.net/project/nsis/NSIS%203/3.10/nsis-3.10.zip?viasf=1"}; // TODO
#else
constexpr std::string_view VSCODE_CONFIG{R"(     {
       "configurations": [
           {
               "name": "Linux OS",
               "compileCommands": "${workspaceFolder}/build/debug/compile_commands.json",
               "includePath": [
                   "${workspaceFolder}/**"
               ],
               "defines": [
                   "_DEBUG",
                   "UNICODE",
                   "_UNICODE"
               ],
               "cStandard": "gnu23",
               "cppStandard": "c++20",
               "intelliSenseMode": "linux-clang-x64"
           }
       ],
       "version": 4
   })"};
constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/aura/releases/latest/download/utool"};
#endif
#endif
