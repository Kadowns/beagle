cmake_minimum_required(VERSION 3.17)
project(beagle)

set(CMAKE_CXX_STANDARD 17)

set(BEAGLE_SOURCES
        src/beagle/engine.cpp
        src/beagle/entity_manager.cpp
        )


add_library(beagle STATIC ${BEAGLE_SOURCES})


target_include_directories(beagle PUBLIC src)

add_subdirectory(external/eagle ${CMAKE_BINARY_DIR}/eagle)

target_link_libraries(beagle PUBLIC eagle)