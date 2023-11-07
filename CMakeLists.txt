cmake_minimum_required(VERSION 3.22)

project(FVMCode)

include(CTest)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES(include)
SET(sources
    src/file_parser.cc
    src/geometry.cc)
ADD_LIBRARY(FVMCode ${sources})

SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -Wextra -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -funroll-loops -funroll-all-loops -fstrict-aliasing")

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(DEBUG)
ENDIF()

IF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(NDEBUG)
ENDIF()

add_subdirectory(tests)
add_subdirectory(scripts)
