# tests/CMakeLists.txt

include_directories(${CMAKE_SOURCE_DIR}/include)

create_test_sourcelist(Tests
    test_driver.cc
    assert_test_01.cc
    point_01.cc
    point_02.cc
    test_helpers_01.cc
    geometry_01.cc
    geometry_02.cc
    unstructured_mesh_01.cc
    unstructured_mesh_02.cc
    unstructured_mesh_03.cc
    skip_foam_header_01.cc
    comment_skipping_01.cc
    )

# Add test driver executable
add_executable(test_driver EXCLUDE_FROM_ALL ${Tests})
set_target_properties(test_driver PROPERTIES
                      COMPILE_DEFINITIONS DEBUG)

target_link_libraries(test_driver FVMCode)

# Remove sources and test driver from list
set(TestsToRun ${Tests})
remove(TestsToRun test_driver.cc)

# Add a test to compile the test driver
add_test(build_test_driver "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target test_driver -j)

# copy over necessary input
file(COPY input01 input02 mesh_1d skip_foam_header_01 comment_skipping_01 DESTINATION ${CMAKE_BINARY_DIR}/tests)

# Add a test for each test
foreach (test ${TestsToRun})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND test_driver ${TName})
    set_tests_properties(${TName} PROPERTIES
                         DEPENDS build_test_driver
                         )
endforeach()

set(TestsThatShouldFail assert_test_01 skip_foam_header_01)

foreach(test ${TestsThatShouldFail})
    set_tests_properties(${test} PROPERTIES WILL_FAIL true)
endforeach()