#Auto Generated Root CMake file by Flick CLI
#Copyright(c) 2025 None.All rights reerved.
cmake_minimum_required(VERSION 3.6...3.31)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(tools.config)
project(flick VERSION 4.2.0 LANGUAGES CXX)
include(app.config)
#@add_find_package Warning: Do not remove this line
    find_package(LibArchive)
    find_package(cpr)
    find_package(fmt)
    find_package(GTest)
    find_package(nlohmann_json)
    find_package(Boost)
# find_path(BARKEEP_INCLUDE_DIRS "barkeep/barkeep.h")
#@add_subproject Warning: Do not remove this line
add_subdirectory(utool)
add_subdirectory(flick)
add_subdirectory(tests)
