#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

int unstructured_mesh_04 (int, char **)
{
    // This test case is for a 2D mesh of 4 blocks, 2x2x1 and overall 1m x 1m x
    // 1m
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "unstructured_mesh_04/points", "unstructured_mesh_04/faces",
        "unstructured_mesh_04/owner", "unstructured_mesh_04/neighbour",
        "unstructured_mesh_04/boundary");

    AssertTest (mesh.n_cells () == 4);

    std::vector<Point<3> > test_points ({ Point<3> (0.25, 0.1, 0.1),
                                          Point<3> (0.9, 0.8, 0.9),
                                          Point<3> (0.01, 0.98, 0.98) });
    std::vector<Point<3> > cell_centres ({ Point<3> (0.25, 0.25, 0.5),
                                           Point<3> (0.75, 0.75, 0.5),
                                           Point<3> (0.25, 0.75, 0.5) });

    for (unsigned int i = 0; i < test_points.size (); i++)
    {
        unsigned int cell_index
            = mesh.get_cell_containing_point (test_points[i]);
        const auto &cell = mesh.get_cell (cell_index);
        AssertTest (close (cell->center ().distance (cell_centres[i]), 0));
    }

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}