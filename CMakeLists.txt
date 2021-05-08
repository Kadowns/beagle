cmake_minimum_required(VERSION 3.17)
project(beagle)

set(CMAKE_CXX_STANDARD 17)

set(BEAGLE_SOURCES
        src/beagle/beagle_global_definitions.h
        src/beagle/engine.cpp
        src/beagle/ecs/entity.cpp
        src/beagle/ecs/job_system.cpp
        src/beagle/ecs/systems/transform_system.cpp
        src/beagle/ecs/systems/camera_system.cpp
        src/beagle/ecs/systems/render_system.cpp
        src/beagle/ecs/systems/mesh_system.cpp
        src/beagle/assets/mesh_pool.cpp
        src/beagle/assets/material_pool.cpp
        src/beagle/assets/shader_pool.cpp
        src/beagle/assets/texture_pool.cpp
        src/beagle/assets/asset_manager.cpp
        )


add_library(beagle STATIC ${BEAGLE_SOURCES})

set(BUILD_STATIC_LIBS TRUE)

add_subdirectory(external/eagle)
add_subdirectory(external/glm)
add_subdirectory(external/stb)

target_include_directories(beagle PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/grapphs/include
        )

target_link_libraries(beagle PUBLIC eagle glm stb)