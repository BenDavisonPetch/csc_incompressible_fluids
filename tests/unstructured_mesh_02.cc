#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

void test ()
{
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "tests/input02/point.txt", "tests/input02/face.txt",
        "tests/input02/cell.txt", "tests/input02/boundary.txt");

    AssertTest(mesh.n_points() == 18);
    AssertTest(mesh.n_faces() == 20);
    AssertTest(mesh.n_cells() == 4);
    AssertTest(mesh.n_boundary_patches() == 3);

    double volume = 0;
    for (auto cell : mesh.cells())
    {
        volume += cell.volume();
    }
    AssertTest(close(volume, 0.01 * 0.1 * 0.1));
    AssertTest(!mesh.get_face(0)->is_boundary());
    Point<3> total_area_vector(0,0,0);
    for (auto face : mesh.faces())
    {
        if (face.is_boundary())
            total_area_vector += face.area_vector();
    }
    AssertTest(close(total_area_vector.norm(), 0));
    AssertTest(mesh.get_cell(0)->neighbour_indices().size() == 2);
}

int main ()
{
    test ();

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}