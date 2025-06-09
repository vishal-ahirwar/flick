#Auto Generated Root CMake file by Flick CLI
#Copyright(c) 2025 None.All rights reerved.
cmake_minimum_required(VERSION 3.6...3.31)
project(flick VERSION 4.1.0 LANGUAGES CXX)
include(res/config.cmake)
#@add_find_package Warning: Do not remove this line
find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(boost_process CONFIG REQUIRED)
find_package(LibArchive REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_path(BARKEEP_INCLUDE_DIRS "barkeep/barkeep.h")
#@add_subproject Warning: Do not remove this line
add_subdirectory(utool)
add_subdirectory(flick)
add_subdirectory(tests)
