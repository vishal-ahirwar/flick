add_executable(
    flick
    src/main/main.cpp
    src/app/app.cpp
    src/deps/deps.cpp
    src/deps/extractor.cpp
    src/deps/libs.cpp
    src/downloader/downloader.cpp
    src/flick/flick.cpp
    src/log/log.cpp
    src/processmanager/processmanager.cpp
    src/projectgenerator/projectgenerator.cpp
    src/projectsetting/projectsetting.cpp
    src/rt/rt.cpp
    src/unittester/unittester.cpp
    src/userinfo/userinfo.cpp
    src/utester/utester.cpp
    src/utils/utils.cpp
    src/unzip/unzip.cpp)# Add your Source Files here
target_link_libraries(flick PRIVATE LibArchive::LibArchive)
if(WIN32)
target_link_libraries(flick  PRIVATE xmllite)
endif()
target_link_libraries(flick PRIVATE cpr::cpr)
target_link_libraries(flick PRIVATE fmt::fmt)
target_link_libraries(flick PRIVATE nlohmann_json::nlohmann_json)
# target_include_directories(flick PRIVATE ${BARKEEP_INCLUDE_DIRS})
target_link_libraries(flick PRIVATE Boost::process)
