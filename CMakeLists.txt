cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

project(CamFinder C CXX)

find_package(Threads)
find_package(OpenSSL)
find_package(tinyxml2)
find_package(Boost
            COMPONENTS
            system
            filesystem
            unit_test_framework
            thread
            REQUIRED)

find_package(PkgConfig REQUIRED)

include_directories(CamFinder ${PROJECT_SOURCE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)

set(PLATFORM_SPECIFIC_LIBS "-lpthread")


add_subdirectory("udp_radar")
add_subdirectory("radar_tool")


