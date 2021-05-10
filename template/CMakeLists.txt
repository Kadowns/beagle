cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../ ${CMAKE_BINARY_DIR}/beagle)

set(APP_SOURCES
        src/template/template_game.cpp
        src/template/ecs/systems/camera_control_system.cpp
        )

add_library(${APP_NAME_LIB} STATIC ${APP_SOURCES})

target_include_directories(${APP_NAME_LIB} PUBLIC src)

target_link_libraries(${APP_NAME_LIB} PUBLIC beagle)

set(SHADERS
        assets/shaders/mesh_phong.frag
        assets/shaders/mesh.vert
        assets/shaders/mesh_color.frag
        )

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_FILENAME ${SHADER} NAME)
    set(SHADER_ABS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    get_filename_component(OUTPUT_DIR ${SHADER_ABS} DIRECTORY)
    set(SHADER_OUTPUT ${OUTPUT_DIR}/${SHADER_FILENAME}.spv)
    set(TARGET_NAME compileShaders-${SHADER_FILENAME})
    add_custom_target(
            ${TARGET_NAME}
            COMMENT "Compiling ${SHADER_ABS} to SPIR_V (${SHADER_OUTPUT})"
            BYPRODUCTS ${SHADER_OUTPUT}
            COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe ${SHADER_ABS} -o ${SHADER_OUTPUT}

    )
    add_dependencies(${APP_NAME_LIB} ${TARGET_NAME})
endforeach ()