cmake_minimum_required(VERSION 3.18)

# Set the project name and version
project(Tako VERSION 1.0.0)

# Set C standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# Automatically grab all files in the src/includes directory
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE INCLUDES "includes/*.h")

# Add the include directories
include_directories(includes)

set(ALL_FILES ${SOURCES} ${HEADERS} ${INCLUDES})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ALL_FILES})


# Create a shared library (DLL)
add_library(Tako SHARED ${ALL_FILES})

# Define TACO_EXPORTS when building the DLL
target_compile_definitions(Tako PRIVATE TACO_EXPORT_DLL)

# Set the library version
set_target_properties(Tako PROPERTIES VERSION ${PROJECT_VERSION})

