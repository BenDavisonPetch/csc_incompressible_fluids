#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include "test_helpers.h"

int unstructured_mesh_01 (int, char**)
{
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "input01/point.txt", "input01/face.txt",
        "input01/cell.txt", "input01/boundary.txt");

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

    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}