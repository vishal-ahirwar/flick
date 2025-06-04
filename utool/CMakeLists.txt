add_executable(
    utool
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/log/log.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/rt/rt.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/downloader/downloader.cpp
    )# Add your Source Files here

target_link_libraries(utool PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(utool PRIVATE LibArchive::LibArchive)
target_link_libraries(utool PRIVATE Boost::process)
target_link_libraries(utool PRIVATE cpr::cpr)
target_link_libraries(utool PRIVATE fmt::fmt)
target_link_libraries(utool PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(utool PRIVATE ${BARKEEP_INCLUDE_DIRS})