cmake_minimum_required(VERSION 3.17)
project(beagle)

set(CMAKE_CXX_STANDARD 17)

set(BEAGLE_SOURCES
        src/beagle/beagle_global_definitions.h
        src/beagle/engine.cpp
        src/beagle/entity.cpp
        src/beagle/renderer/renderer.cpp
        )


add_library(beagle STATIC ${BEAGLE_SOURCES})



set(BUILD_STATIC_LIBS TRUE)

add_subdirectory(external/eagle)
add_subdirectory(external/glm)

target_include_directories(beagle PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
        )

target_link_libraries(beagle PUBLIC eagle glm)