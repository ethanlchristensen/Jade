cmake_minimum_required(VERSION 3.22)
project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

message("cmake module path ${CMAKE_MODULE_PATH}")

find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
find_library(MPG123_LIBRARY mpg123)

message(STATUS "MPG123 library: ${MPG123_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)

find_package(DPP CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_library(FFMPEG_LIB avcodec avformat avutil avfilter)

add_executable(${PROJECT_NAME} src/main.cpp
        include/commands/commands.h
        include/apis/apis.h
        src/commands/nolan.cpp
        src/commands/clear.cpp
        src/commands/echo.cpp
        src/commands/echo.cpp
        src/commands/llmCommands.cpp
        src/commands/voice.cpp
        src/commands/say.cpp
        src/apis/helpers.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${DPP_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
        ${MPG123_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
        ${DPP_LIBRARIES}
        ${CURL_LIBRARIES}
        ${MPG123_LIBRARY}
        fmt::fmt
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
