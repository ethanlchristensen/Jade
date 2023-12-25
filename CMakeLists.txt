cmake_minimum_required(VERSION 3.22)
project(Jade VERSION 1.0 DESCRIPTION "Jade :-)")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# find dpp
find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS ${DPP_ROOT_DIR})
find_library(DPP_LIBRARIES NAMES dpp "libdpp.a" HINTS ${DPP_ROOT_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)
find_package(DPP)

if (DPP_FOUND)
    message("DPP FOUND!")
else()
    message("DPP NOT FOUND!")
endif()

add_executable(${PROJECT_NAME} src/main.cpp
        src/commands/commands.h
        src/utils/utils.h
        src/commands/nolan.cpp
        src/commands/clear.cpp
        src/utils/log.cpp
)

target_link_libraries(${PROJECT_NAME} ${DPP_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DPP_INCLUDE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
