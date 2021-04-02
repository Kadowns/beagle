cmake_minimum_required(VERSION 3.17)
project(template_app)

set(APP_NAME template_app)
set(APP_NAME_LIB ${APP_NAME}_lib)
set(APP_NAME_EXE ${APP_NAME})
add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_executable(${APP_NAME_EXE} main.cpp)

target_link_libraries(${APP_NAME_EXE} PRIVATE ${APP_NAME_LIB})

add_custom_target(
        copy_assets_folder
        COMMENT "Copying assets folder"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/
)

add_dependencies(copy_assets_folder ${APP_NAME_LIB})
add_dependencies(${APP_NAME_EXE} copy_assets_folder)