cmake_minimum_required(VERSION 3.18)

# Set the project name and version
project(Tako VERSION 1.0.0)

# Set C standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Automatically grab all files in the src/includes directory
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE INCLUDES "includes/*.h")
file(GLOB VS_SHADER "src/shaders/compositor_vs.hlsl")
file(GLOB PS_SHADER "src/shaders/compositor_ps.hlsl")


# Add the include directories
include_directories(includes)
include_directories(
    $<IF:$<CONFIG:Debug>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug,>
    $<IF:$<CONFIG:Release>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release,>
    $<IF:$<CONFIG:RelWithDebInfo>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo,>
    $<IF:$<CONFIG:MinSizeRel>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel,>
)

set(ALL_FILES ${SOURCES} ${HEADERS} ${INCLUDES} ${VS_SHADER} ${PS_SHADER})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ALL_FILES})


# Create a shared library (DLL)
add_library(Tako SHARED ${ALL_FILES})

# Define TACO_EXPORTS when building the DLL
target_compile_definitions(Tako PRIVATE TACO_EXPORT_DLL)
target_compile_definitions(Tako PRIVATE UNICODE)

# Set the library version
set_target_properties(Tako PROPERTIES VERSION ${PROJECT_VERSION})

# Link d3d11
target_link_libraries(Tako PRIVATE d3d11)

# Set the VS shader properties
set_property(SOURCE ${VS_SHADER} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${VS_SHADER} PROPERTY VS_SHADER_ENTRYPOINT "VS_Main")
set_property(SOURCE ${VS_SHADER} PROPERTY VS_SHADER_OUTPUT_HEADER_FILE "$(OutDir)/data/%(Filename).h")
set_property(SOURCE ${VS_SHADER} PROPERTY VS_SHADER_OBJECT_FILE_NAME "")

# Set the PS shader properties
set_property(SOURCE ${PS_SHADER} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${PS_SHADER} PROPERTY VS_SHADER_ENTRYPOINT "PS_Main")
set_property(SOURCE ${PS_SHADER} PROPERTY VS_SHADER_OUTPUT_HEADER_FILE "$(OutDir)/data/%(Filename).h")
set_property(SOURCE ${PS_SHADER} PROPERTY VS_SHADER_OBJECT_FILE_NAME "")

