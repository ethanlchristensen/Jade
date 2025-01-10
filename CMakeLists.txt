cmake_minimum_required(VERSION 3.22)
project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

message("cmake module path ${CMAKE_MODULE_PATH}")

find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
find_library(MPG123_LIBRARY mpg123)
find_path(CPPCODEC_INCLUDE_DIRS "cppcodec/base32_crockford.hpp")
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
        src/apis/helpers.cpp
        src/utils/jade_queue.cpp
        include/utils/jade_queue.h
        src/commands/voice/play_command.cpp
        include/commands/voice/play_command.h
        src/commands/voice/skip_command.cpp
        include/commands/voice/skip_command.h
        src/commands/voice/pause_command.cpp
        include/commands/voice/pause_command.h
        src/commands/voice/resume_command.cpp
        include/commands/voice/resume_command.h
        src/utils/voice/stream_audio.cpp
        include/utils/voice/stream_audio.h
        src/commands/voice/join_command.cpp
        include/commands/voice/join_command.h
        src/commands/voice/leave_command.cpp
        include/commands/voice/leave_command.h
        src/utils/jade_embed.cpp
        include/utils/jade_embed.h
        src/utils/jade_slash.cpp
        include/utils/jade_slash.h
        src/commands/voice/play_test_command.cpp
        include/commands/voice/play_test_command.h
        src/utils/youtube/youtube_helpers.cpp
        include/utils/youtube/youtube_helpers.h
        src/utils/jade_util.cpp
        include/utils/jade_util.h
        src/commands/ollama/chat_command.cpp
        include/commands/ollama/chat_command.h
        src/utils/ollama/ollama.cpp
        include/utils/ollama/ollama.h
        include/utils/ollama/ollama.h
        src/commands/ollama/describe_command.cpp
        include/commands/ollama/describe_command.h
        include/utils/env.h
        src/utils/env.cpp
        src/commands/reply_command.cpp
        include/commands/reply_command.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${DPP_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
        ${MPG123_INCLUDE_DIR}
        ${CPPCODEC_INCLUDE_DIRS}
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
