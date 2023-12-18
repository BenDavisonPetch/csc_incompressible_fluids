find_package(Eigen3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${EIGEN3_INCLUDE_DIR})

add_executable(transient_laplacian transient_laplacian/transient_laplacian.cc)
target_link_libraries(transient_laplacian FVMCode)
file(COPY transient_laplacian/constant DESTINATION ${CMAKE_BINARY_DIR}/scripts/transient_laplacian/)