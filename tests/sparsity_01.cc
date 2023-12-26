#include <FVMCode/file_parser.h>
#include <FVMCode/sparsity/sparse_matrix.h>
#include <FVMCode/sparsity/sparsity_pattern.h>

#include "test_helpers.h"

int sparsity_01 (int, char **)
{
    // This test case is for a 2D mesh of 4 blocks, 2x2x1 and overall 1m x 1m x
    // 1m
    using namespace FVMCode;

    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "unstructured_mesh_04/points", "unstructured_mesh_04/faces",
        "unstructured_mesh_04/owner", "unstructured_mesh_04/neighbour",
        "unstructured_mesh_04/boundary");

    std::cout << "Parsed mesh" << std::endl;

    SparsityPattern sp (mesh);

    std::cout << "Constructed SparsityPattern" << std::endl;

    AssertTest (sp.n_eqns () == 4);
    AssertTest (sp.n_off_diagonal_entries () == 8);
    AssertTest (sp.matrix_band () == 2);

    std::cout << "\tTested SparsityPattern" << std::endl;

    SparseMatrix matrix (sp);

    std::cout << "Constructed SparseMatrix" << std::endl;

    // The matrix will look like:
    /*
     * 4   1   2   .
     * 1   5   .   3
     * 2   .   6   0
     * .   3   0   7
     */
    matrix (0, 0) = 4;
    matrix (1, 1) = 5;
    matrix (2, 2) = 6;
    matrix (3, 3) = 7;
    matrix (0, 1) = 1;
    matrix (1, 0) = 1;
    matrix (0, 2) = 2;
    matrix (2, 0) = 2;
    matrix (1, 3) = 3;
    matrix (3, 1) = 3;

    std::cout << "Filled matrix" << std::endl;

    AssertTest (matrix.symmetric ());
    AssertTest (matrix.diagonally_dominant ());
    AssertTest (matrix.spd ());

    std::cout << "Tested matrix characteristics" << std::endl;

    VectorXd x(4);
    x << 1, 2, 3, 4;
    VectorXd result(4);
    matrix.vmult(x, result);

    AssertTest(result(0) == 12);
    AssertTest(result(1) == 23);
    AssertTest(result(2) == 20);
    AssertTest(result(3) == 34);

    std::cout << "Tested vmult" << std::endl;

    result << 1, 2, 3, 4;
    matrix.vmult_add(x, result);
    AssertTest(result(0) == 13);
    AssertTest(result(1) == 25);
    AssertTest(result(2) == 23);
    AssertTest(result(3) == 38);

    std::cout << "Tested vmult_add" << std::endl;

    return EXIT_SUCCESS;
}