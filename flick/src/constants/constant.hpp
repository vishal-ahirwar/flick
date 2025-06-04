#ifndef _CONSTANT_
#define _CONSTANT_
#include <string>
static std::string VCPKG_TRIPLET{"default"};

#if defined(_WIN32)
#include <windows.h>
#define USERNAME "USERPROFILE" // Windows environment variable
#elif defined(__linux__)
#include <unistd.h>
#define USERNAME "USER" // Linux environment variable
#elif defined(__APPLE__)
#include <unistd.h> // For macOS
#include "Flick.hpp"
#define USERNAME "USER" // macOS environment variable
#endif
#ifdef _WIN32
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif

enum class Language
{
  CXX,
  C,
  NONE
};
enum class ProjectType
{
  NONE,
  EXECUTABLE,
  LIBRARY
};

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
constexpr std::string_view UNIT_TEST_CODE[]{
    R"(
#include <gtest/gtest.h>
int add(int a,int b){return a+b;};
int subtract(int a,int b){return a-b;};
TEST(CalculatorTest, Addition) {
    EXPECT_EQ(add(3, 4), 7);
    EXPECT_EQ(add(-1, 5), 4);
}

TEST(CalculatorTest, Subtraction) {
    EXPECT_EQ(subtract(10, 3), 7);
    EXPECT_EQ(subtract(7, 7), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
  )",
    R"(#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
int add(int a,int b){return a+b;};
int subtract(int a,int b){return a-b;};

static void test_add(void **state) {
    (void) state; // Unused
    assert_int_equal(add(3, 4), 7);
    assert_int_equal(add(-1, 5), 4);
}

static void test_subtract(void **state) {
    (void) state; // Unused
    assert_int_equal(subtract(10, 3), 7);
    assert_int_equal(subtract(7, 7), 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add),
        cmocka_unit_test(test_subtract),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
})"};

constexpr std::string_view GITIGNORE_CODE{
    R"(.vs
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
.compile.logs
)"};

constexpr std::string_view CONFIG_CMAKE[]{
    R"(
#Auto Generated Root CMake file by Flick CLI
#@COPYRIGHT
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(STATIC_LINK "Enable static linking" ON)
option(ENABLE_TESTS "GTests" OFF)
if(STATIC_LINK)
  set(BUILD_SHARED_LIBS OFF)
  if (WIN32)
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  else()
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  endif()
else()
  set(BUILD_SHARED_LIBS ON)
endif()
set(COMPANY "@DeveloperName")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} ${COMPANY}.")
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
configure_file(@config_in @config_h)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling secure coding features for Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic        # General warnings
        -Wshadow -Wold-style-cast       # Detect potential issues
        -Wcast-align -Wnull-dereference # Runtime safety
        -Wformat=2 -Wformat-security    # Secure formatting
        -fstack-protector-strong        # Stack protection
        -D_FORTIFY_SOURCE=2             # Buffer security
        -fno-common                     # Avoid common symbol issues
        #-Werror                         # Treat warnings as errors
    )
endif()
)",
    R"(
#Auto Generated Root CMake file by Flick CLI
#@COPYRIGHT
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(STATIC_LINK "Enable static linking" ON)
option(ENABLE_TESTS "CMOCKA Test" OFF)
if(STATIC_LINK)
  message(STATUS "Static linking enabled")
  set(BUILD_SHARED_LIBS OFF)
  if (WIN32)
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  else()
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  endif()
endif()
set(COMPANY "@DeveloperName")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} ${COMPANY}.")
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
configure_file(@config_in @config_h)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(STATUS "Enabling secure coding features for Clang")
  add_compile_options(
    -Wall -Wextra -Wpedantic        # General warnings
    -Wshadow -Wold-style-cast       # Detect potential issues
    -Wcast-align -Wnull-dereference # Runtime safety
    -Wformat=2 -Wformat-security    # Secure formatting
    -fstack-protector-strong        # Stack protection
    -D_FORTIFY_SOURCE=2             # Buffer security
    -fno-common                     # Avoid common symbol issues
    #-Werror                         # Treat warnings as errors
    )
endif()
)"};

static std::string MAIN_CODE[]{R"(//Auto Genrated C++ file by Flick CLI
//@COPYRIGHT
#include<iostream>
_HEADER_
int main(int argc,char*argv[])
{
    std::cerr << "Hello, " << Project::COMPANY_NAME << std::endl;
    std::cerr << Project::PROJECT_NAME << " v" << Project::VERSION_STRING << std::endl;
    std::cerr << Project::COPYRIGHT_STRING << std::endl;
    for(int i=0;i<argc;++i){
      std::cerr<<argv[i]<<std::endl;
    }
    return 0;
}
)",
 R"(//Auto Genrated C file by Flick CLI
//@COPYRIGHT
#include<stdio.h>
_HEADER_
int main(int argc,char*argv[])
{
    printf("%s v%s %s\n",PROJECT_NAME,VERSION_STRING,COMPANY_NAME);
    for(int i=0;i<argc;++i){
      puts(argv[i]);
    }
    return 0;
}
)"};

constexpr std::string_view VIM_CONFIG{R"()"};

constexpr std::string_view CMAKE_PRESETS[]{R"(
{
  "version": 2,
  "configurePresets": [
    {
      "name": "default",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_CXX_COMPILER": "clang++",
        "STATIC_LINK": false
      }
    },
    {
      "name": "windows-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-windows-static",
        "STATIC_LINK": true
      }
    },
    {
      "name": "linux-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-linux",
        "STATIC_LINK": true
      }
    },
    {
      "name": "osx-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-osx",
        "STATIC_LINK": true
      }
    }
  ]
})",
                                           R"(
{
  "version": 2,
  "configurePresets": [
    {
      "name": "default",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_C_COMPILER": "clang",
        "STATIC_LINK": false
      }
    },
    {
      "name": "windows-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-windows-static",
        "STATIC_LINK": true
      }
    },
    {
      "name": "linux-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-linux",
        "STATIC_LINK": true
      }
    },
    {
      "name": "osx-static-build",
      "inherits": "default",
      "cacheVariables": {
        "VCPKG_TARGET_TRIPLET": "x64-osx",
        "STATIC_LINK": true
      }
    }
  ]
})"};
#ifdef _WIN32
constexpr std::string_view VSCODE_CONFIG{R"(    {
       "configurations": [
           {
               "name": "Windows OS",
               "compileCommands": "${workspaceFolder}/build/default/compile_commands.json",
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

constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/Flick/releases/latest/download/utool.exe"};
constexpr std::string_view COMPILER_URL{"https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.7/clang+llvm-19.1.7-x86_64-pc-windows-msvc.tar.xz"};
constexpr std::string_view CMAKE_URL{"https://github.com/Kitware/CMake/releases/download/v3.31.5/cmake-3.31.5-windows-x86_64.zip"};
constexpr std::string_view NINJA_URL{"https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"};
constexpr std::string_view VS_BUILD_TOOLS_INSTALLER_URL{"https://aka.ms/vs/17/release/vs_BuildTools.exe"};

#else
constexpr std::string_view VSCODE_CONFIG{R"(     {
       "configurations": [
           {
               "name": "Linux OS",
               "compileCommands": "${workspaceFolder}/build/default/compile_commands.json",
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
constexpr std::string_view UPDATER_URL{"https://github.com/vishal-ahirwar/Flick/releases/latest/download/utool"};
#endif
#endif

