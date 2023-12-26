find_package(Eigen3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${EIGEN3_INCLUDE_DIR})

# ============== Transient Laplacian ==============
add_executable(transient_laplacian transient_laplacian/transient_laplacian.cc)
target_link_libraries(transient_laplacian FVMCode)

# Necessary files
make_directory(${CMAKE_BINARY_DIR}/scripts/transient_laplacian/output/case1)
make_directory(${CMAKE_BINARY_DIR}/scripts/transient_laplacian/plots)
file(COPY transient_laplacian/constant DESTINATION ${CMAKE_BINARY_DIR}/scripts/transient_laplacian/)

# Add run command
set_property(TARGET transient_laplacian
    PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/transient_laplacian)
add_custom_target(run_transient_laplacian
                  COMMAND transient_laplacian
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/transient_laplacian
                  )
# add postprocessing
add_custom_command(
    TARGET run_transient_laplacian
    POST_BUILD
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/transient_laplacian/plot.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/transient_laplacian
    )

# ============== Convection Diffusion ==============
add_executable(convection_diffusion convection_diffusion/convection_diffusion.cc)
target_link_libraries(convection_diffusion FVMCode)

# Necessary files
file(COPY convection_diffusion/constant DESTINATION ${CMAKE_BINARY_DIR}/scripts/convection_diffusion/)

# Add run command
set_property(TARGET convection_diffusion
    PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/convection_diffusion)
add_custom_target(run_convection_diffusion
                  COMMAND convection_diffusion
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/convection_diffusion
                  )
add_custom_command(
    TARGET run_convection_diffusion
    POST_BUILD
    COMMAND openfoam2306 paraFoam
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/scripts/convection_diffusion
    )