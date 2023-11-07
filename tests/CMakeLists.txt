# tests/CMakeLists.txt

include_directories(${CMAKE_SOURCE_DIR}/include)

create_test_sourcelist(Tests
    test_driver.cc
    point_01.cc
    point_02.cc
    test_helpers_01.cc
    geometry_01.cc
    geometry_02.cc
    unstructured_mesh_01.cc
    unstructured_mesh_02.cc
    unstructured_mesh_03.cc
    )

# Add test driver executable
add_executable(test_driver ${Tests} ${sources})

target_link_libraries(test_driver FVMCode)

# Remove sources and test driver from list
set(TestsToRun ${Tests})
remove(TestsToRun test_driver.cc)

# Add a test to compile the test driver
add_test(build_test_driver "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target test_driver)

# Add a test for each test
foreach (test ${TestsToRun})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND test_driver ${TName})
    set_tests_properties(${TName} PROPERTIES DEPENDS build_test_driver)
endforeach()