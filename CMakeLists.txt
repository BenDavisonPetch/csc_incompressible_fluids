cmake_minimum_required(VERSION 3.22)

project(FVMCode)

include(CTest)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FILE(GLOB sources src/*.cc)

INCLUDE_DIRECTORIES(include)

SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -Wextra -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -funroll-loops -funroll-all-loops -fstrict-aliasing")

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(DEBUG)
ENDIF()

IF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(NDEBUG)
ENDIF()

ADD_EXECUTABLE(test_point_01 EXCLUDE_FROM_ALL tests/point_01.cc)
ADD_EXECUTABLE(test_point_02 EXCLUDE_FROM_ALL tests/point_02.cc)
ADD_EXECUTABLE(test_helpers_01 EXCLUDE_FROM_ALL tests/test_helpers_01.cc)
ADD_EXECUTABLE(test_geometry_01 EXCLUDE_FROM_ALL tests/geometry_01.cc)
ADD_EXECUTABLE(test_geometry_02 EXCLUDE_FROM_ALL tests/geometry_02.cc src/geometry.cc)
ADD_EXECUTABLE(test_unstructured_mesh_01 EXCLUDE_FROM_ALL tests/unstructured_mesh_01.cc ${sources})
ADD_EXECUTABLE(test_unstructured_mesh_02 EXCLUDE_FROM_ALL tests/unstructured_mesh_02.cc ${sources})
ADD_EXECUTABLE(test_unstructured_mesh_03 EXCLUDE_FROM_ALL tests/unstructured_mesh_03.cc ${sources})

ADD_TEST(NAME test_point_01 COMMAND test_point_01)
ADD_TEST(NAME test_point_02 COMMAND test_point_02)
ADD_TEST(NAME test_helpers_01 COMMAND test_point_02)
ADD_TEST(NAME test_geometry_01 COMMAND test_point_02)
ADD_TEST(NAME test_geometry_02 COMMAND test_point_02)
ADD_TEST(NAME test_unstructured_mesh_01 COMMAND test_point_02)
ADD_TEST(NAME test_unstructured_mesh_02 COMMAND test_point_02)
ADD_TEST(NAME test_unstructured_mesh_03 COMMAND test_point_02)
