cmake_minimum_required(VERSION 3.6...3.31)
project(aura VERSION 2.5.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPANY "Vishal ahirwar")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} Vishal Ahirwar and all Contributors. All rights reserved.")

# Check if the compiler is Clang
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project requires Clang as the compiler. Please set Clang as the compiler.")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project requires Clang as the compiler. Please set Clang as the compiler.")
endif()

include_directories(src ${CMAKE_BINARY_DIR} external/json ${CMAKE_SOURCE_DIR}/external/cpr/install/include)
set(cpr_DIR ${CMAKE_SOURCE_DIR}/external/cpr)
set(fmt_DIR ${CMAKE_SOURCE_DIR}/external/fmt/install/lib/cmake/fmt)
find_package(cpr REQUIRED)
find_package(fmt REQUIRED)

configure_file(auraConfig.h.in auraConfig.h)
file(GLOB SOURCES "src/*.cc" "src/*/*.cc")
add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(utool utool/main.cc src/downloader/downloader.cc src/rt/rt.cc src/userinfo/userinfo.cc ./src/log/log.cc)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/cpr/install/lib)
target_link_directories(utool PUBLIC ${CMAKE_SOURCE_DIR}/external/cpr/install/lib)

target_link_libraries(${PROJECT_NAME} PUBLIC cpr fmt::fmt curl z ws2_32 bcrypt crypt32)
target_link_libraries(utool PUBLIC cpr fmt::fmt curl z ws2_32 bcrypt crypt32)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR ${COMPANY})
include(CPack)
