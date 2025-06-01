find_package(GTest CONFIG REQUIRED)
add_executable(
    tests
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/userinfo/userinfo.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/projectgenerator/projectgenerator.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/log/log.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/projectsetting/projectsetting.cpp
    )# Add your Source Files here

target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(tests PRIVATE LibArchive::LibArchive)
target_link_libraries(tests PRIVATE Boost::process)
target_link_libraries(tests PRIVATE cpr::cpr)
target_link_libraries(tests PRIVATE fmt::fmt)
target_link_libraries(tests PRIVATE nlohmann_json::nlohmann_json)