cmake_minimum_required(VERSION 3.22)
project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

message("cmake module path ${CMAKE_MODULE_PATH}")

# find dpp
find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)
find_package(DPP)

# find curlpp
find_path(CURLPP_INCLUDE_DIR NAMES curlpp/curlpp.h HINTS ${CURLPP_ROOT_DIR})

message("curlpp include dir: ${CURLPP_INCLUDE_DIR}")
message("curlpp root dir: ${CURLPP_ROOT_DIR}")
#find_library(CURLPP_LIBRARIES NAMES curlpp "libcurlpp.a" HINTS ${CURLPP_ROOT_DIR})
#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(curlpp DEFAULT_MSG CURLPP_LIBRARIES CURLPP_INCLUDE_DIR)
#find_package(curlpp)

if (!DPP_FOUND)
    message("DPP NOT FOUND!")
endif()

add_executable(${PROJECT_NAME} src/main.cpp
        src/commands/commands.h
        src/commands/nolan.cpp
        src/commands/clear.cpp
        src/commands/echo.cpp
        src/commands/echo.cpp
        src/apis/apis.h
        src/apis/generate.cpp
        src/commands/chat.cpp
)

target_link_libraries(${PROJECT_NAME} ${DPP_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DPP_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} ${CURLPP_LIBRARIES})
#target_include_directories(${PROJECT_NAME} PUBLIC ${CURLPP_INCLUDE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
