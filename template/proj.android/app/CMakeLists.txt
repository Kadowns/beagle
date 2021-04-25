cmake_minimum_required(VERSION 3.18.4)
project(template)

set(APP_NAME template)
set(APP_NAME_LIB ${APP_NAME}_lib)
set(APP_NAME_EXE ${APP_NAME})

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME_LIB})

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(${APP_NAME_EXE} SHARED
        src/main/cpp/main.cpp
        )

target_link_libraries(${APP_NAME_EXE}
        PUBLIC
        -Wl,--whole-archive
        ${APP_NAME_LIB}
        -Wl,--no-whole-archive
        )