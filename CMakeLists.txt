cmake_minimum_required(VERSION 3.22)
find_package(Eigen3 REQUIRED)

project(FVMCode)

include(CTest)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FILE(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json SYMBOLIC)

INCLUDE_DIRECTORIES(include ${EIGEN3_INCLUDE_DIR})
SET(sources
    src/file_parser.cc
    src/geometry.cc
    src/output.cc
    src/input.cc)
ADD_LIBRARY(FVMCode ${sources})

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
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
