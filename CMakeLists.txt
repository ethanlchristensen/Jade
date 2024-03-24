# cmake_minimum_required(VERSION 3.22)
# project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# list(APPEND CMAKE_PREFIX_PATH "/usr/bin/mpg123")

# include_directories("/usr/bin/mpg123/includes")
# link_directories("/usr/bin/mpg123/lib")

# message("cmake module path ${CMAKE_MODULE_PATH}")

# # find external libraries
# find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
# find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
# find_path(MPG123_INCLUDE_DIR NAMES mpg123.h HINTS ${MPG123_ROOT_DIR})
# find_library(MPG123_LIBRARIES NAMES mpg123 HINTS ${MPG123_ROOT_DIR})
# include(FindPackageHandleStandardArgs)

# # file(COPY ${CMAKE_SOURCE_DIR}/src/commands/audio DESTINATION ${CMAKE_BINARY_DIR})

# find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)

# find_package(DPP CONFIG REQUIRED)
# find_package(fmt CONFIG REQUIRED)
# find_package(CURL REQUIRED)
# find_package(mpg123 REQUIRED)
# find_package(PkgConfig REQUIRED)
# find_package(opus REQUIRED)

# pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
#         libavdevice
#         libavfilter
#         libavformat
#         libavcodec
#         libswresample
#         libswscale
#         libavutil
# )

# add_executable(${PROJECT_NAME} src/main.cpp
#         src/commands/commands.h
#         src/commands/nolan.cpp
#         src/commands/clear.cpp
#         src/commands/echo.cpp
#         src/commands/echo.cpp
#         src/apis/apis.h
#         src/commands/llmCommands.cpp
#         src/apis/helpers.cpp
#         src/commands/voice.cpp
#         src/commands/say.cpp
# )

# target_link_libraries(${PROJECT_NAME} PUBLIC
#         ${CMAKE_THREAD_LIBS_INIT}
#         ${DPP_LIBRARIES}
#         ${CURL_LIBRARIES}
# 	${MPG123_LIBRARIES}
# 	fmt::fmt
#         PkgConfig::LIBAV
#         Opus::opus
#         odbc32
# )
# target_include_directories(${PROJECT_NAME} PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
#         ${DPP_INCLUDE_DIR}
#         ${OPUS_INCLUDE_DIRS}
# 	${CURL_INCLUDE_DIR}
# 	${MPG123_INCLUDE_DIR}
# )

# set_target_properties(${PROJECT_NAME} PROPERTIES
#         CXX_STANDARD 17
#         CXX_STANDARD_REQUIRED ON
# )

##########################################################################################################################
cmake_minimum_required(VERSION 3.22)
project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Adjust these paths according to your mpg123 installation
# set(MPG123_INCLUDE_DIR "/path/to/mpg123/include")
# set(MPG123_LIBRARY_DIR "/path/to/mpg123/lib")

message("cmake module path ${CMAKE_MODULE_PATH}")

# Find external libraries
find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
find_library(LIBMPG123 mpg123)
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)

find_package(DPP CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(CURL REQUIRED)
# Commented out the find_package call for mpg123 since we're setting paths manually
find_package(PkgConfig REQUIRED)
# find_package(opus REQUIRED)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavdevice
        libavfilter
        libavformat
        libavcodec
        libswresample
        libswscale
        libavutil
)

add_executable(${PROJECT_NAME} src/main.cpp
        src/commands/commands.h
        src/commands/nolan.cpp
        src/commands/clear.cpp
        src/commands/echo.cpp
        src/commands/echo.cpp
        src/apis/apis.h
        src/commands/llmCommands.cpp
        src/apis/helpers.cpp
        src/commands/voice.cpp
        src/commands/say.cpp
)

# Assuming mpg123 provides a pkg-config file, you could alternatively use pkg_search_module to find mpg123
# pkg_search_module(MPG123 REQUIRED mpg123)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${DPP_INCLUDE_DIR}
        # ${OPUS_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIR}
        ${MPG123_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
        ${DPP_LIBRARIES}
        ${CURL_LIBRARIES}
        fmt::fmt
        ${MPG123_LIBRARIES}
        PkgConfig::LIBAV

)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
