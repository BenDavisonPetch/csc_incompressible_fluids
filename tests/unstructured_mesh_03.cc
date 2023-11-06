#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

void test ()
{
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

    for (const auto& cell : mesh.cells()) {
        AssertTest(close(cell.volume(), 0.005*0.1*0.01));
    }
}

int main ()
{
    test ();

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}