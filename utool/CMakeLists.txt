add_executable(
    utool
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/log/log.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/rt/rt.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/downloader/downloader.cpp
    ${CMAKE_SOURCE_DIR}/flick/src/unzip/unzip.cpp
    )# Add your Source Files here
target_link_libraries(utool PRIVATE LibArchive::LibArchive)
target_link_libraries(utool PRIVATE cpr::cpr)
target_link_libraries(utool PRIVATE fmt::fmt)
if(WIN32)
target_link_libraries(utool  PRIVATE xmllite)
endif()