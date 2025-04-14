cmake_minimum_required(VERSION 3.6...3.31)
project(aura VERSION 4.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(COMPANY "Vishal ahirwar")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright(c) ${CURRENT_YEAR} Vishal Ahirwar and all Contributors. All rights reserved.")

option(STATIC_LINK "Enable static linking" ON)
option(ENABLE_TESTS "Enable tests" ON)

if(STATIC_LINK)
    message(STATUS "Static linking enabled")
    # Ensure static runtime linking on Windows
    if (WIN32)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()
#@find
find_package(boost_process CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(src ${CMAKE_BINARY_DIR})
configure_file(res/auraConfig.h.in auraConfig.h)
file(GLOB SOURCES "src/*.cc" "src/*/*.cc")

    add_executable(${PROJECT_NAME} ${SOURCES} ${CMAKE_SOURCE_DIR}/src/main/main.cpp)
    add_executable(utool utool/main.cc src/downloader/downloader.cc src/rt/rt.cc src/userinfo/userinfo.cc ./src/log/log.cc)
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling secure coding features for Clang")
    
    target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic        # General warnings
    -Wshadow -Wnon-virtual-dtor      # Detect OOP issues
    -Wold-style-cast -Wcast-align    # Avoid unsafe casting
    -Wnull-dereference -Wdouble-promotion  # Runtime safety
    -Wformat=2 -Wformat-security     # Secure printf-like formatting
    -fstack-protector-strong         # Stack protection
    -D_FORTIFY_SOURCE=2              # Fortify source (buffer security)
    -fno-common                      # Disallow common global variables
    )
    endif()
    
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    #@link
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::process)
    target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(utool PRIVATE cpr::cpr)
    target_link_libraries(utool PRIVATE fmt::fmt)
    target_link_libraries(utool PRIVATE Boost::process)
    include(InstallRequiredSystemLibraries)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VENDOR ${COMPANY})
    include(CPack)

    find_package(GTest)
    enable_testing()
    file(GLOB TEST_SOURCES "tests/*.cc")
    add_executable(tests ${TEST_SOURCES} ${SOURCES})
    target_link_libraries(tests GTest::GTest GTest::Main cpr::cpr fmt::fmt nlohmann_json::nlohmann_json Boost::process)
    add_test(NAME MyUnitTests COMMAND tests)

