#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

void test ()
{
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "tests/input01/point.txt", "tests/input01/face.txt",
        "tests/input01/cell.txt", "tests/input01/boundary.txt");

    AssertTest (mesh.n_points () == 8);
    AssertTest (mesh.n_faces () == 6);
    AssertTest (mesh.n_boundary_patches () == 1);
    AssertTest (close (mesh.get_cell (0)->volume (), 0.5 * 0.5 * 0.5));
    AssertTest (close (
        mesh.get_cell (0)->center ().distance (Point<3> (0.25, 0.25, 0.25)),
        0));
    for (unsigned int i = 0; i < 6; i++)
    {
        AssertTest (close (mesh.get_face (i)->center ().distance (
                               mesh.get_cell (0)->center ()),
                           0.25));
        AssertTest (mesh.get_face (i)->is_boundary ());
        AssertTest (mesh.get_face (i)->boundary_id () == 0);
        AssertTest (close (mesh.get_face (i)->area (), 0.25));
        AssertTest (mesh.get_face (i)->n_vertices () == 4);
    }
    AssertTest (close (
        mesh.get_face (0)->area_vector ().distance (Point<3> (0, 0, -0.25)),
        0));
    AssertTest (close (
        mesh.get_face (1)->area_vector ().distance (Point<3> (0, -0.25, 0)),
        0));
    AssertTest (close (
        mesh.get_face (2)->area_vector ().distance (Point<3> (0.25, 0, 0)),
        0));
    AssertTest (close (
        mesh.get_face (3)->area_vector ().distance (Point<3> (0, 0.25, 0)),
        0));
    AssertTest (close (
        mesh.get_face (4)->area_vector ().distance (Point<3> (-0.25, 0, 0)),
        0));
    AssertTest (close (
        mesh.get_face (5)->area_vector ().distance (Point<3> (0, 0, 0.25)),
        0));
    AssertTest (close (
        mesh.get_cell (0)->center ().distance (Point<3> (0.25, 0.25, 0.25)),
        0));
}

int main ()
{
    test ();

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}