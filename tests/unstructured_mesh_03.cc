#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

void test ()
{
    // This test case is for a 1D mesh of 20 blocks, each with extent (0.005
    // 0.1, 0.01), and stacked in the x direction
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "tests/mesh_1d/points", "tests/mesh_1d/faces",
        "tests/mesh_1d/owner", "tests/mesh_1d/neighbour",
        "tests/mesh_1d/boundary");

    AssertTest (mesh.n_cells () == 20);
    AssertTest (mesh.n_boundary_patches () == 3);

    AssertTest (mesh.get_patches ()[0].name == "inlet");
    AssertTest (mesh.get_patches ()[0].type == wall);
    AssertTest (mesh.get_patches ()[0].n_faces == 1);
    AssertTest (mesh.get_patches ()[0].start_face == 19);

    AssertTest (mesh.get_patches ()[1].name == "outlet");
    AssertTest (mesh.get_patches ()[1].type == wall);
    AssertTest (mesh.get_patches ()[1].n_faces == 1);
    AssertTest (mesh.get_patches ()[1].start_face == 20);

    AssertTest (mesh.get_patches ()[2].name == "sides");
    AssertTest (mesh.get_patches ()[2].type == empty);
    AssertTest (mesh.get_patches ()[2].n_faces == 80);
    AssertTest (mesh.get_patches ()[2].start_face == 21);

    for (const auto &cell : mesh.cells ())
    {
        AssertTest (close (cell.volume (), 0.005 * 0.1 * 0.01));
    }

    for (const BoundaryPatch& boundary : mesh.get_patches())
    {
        for (unsigned int f = boundary.start_face; f < boundary.start_face + boundary.n_faces; f++)
        {
            const auto& face = mesh.get_face(f);
            AssertTest(face->is_boundary());
            AssertTest(face->interpolation_factor() == 1.);
            AssertTest(close(face->delta(), 2. / 0.1) || close(face->delta(), 2. / 0.01));
        }
    }

    for (const Face<3>& face : mesh.faces()) {
        if (face.is_boundary()) continue;
        AssertTest(close(face.delta(), 1. / 0.005));
        AssertTest(close(face.interpolation_factor(), 0.5));
    }
    AssertTest(false);
}

int main ()
{
    test ();

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}