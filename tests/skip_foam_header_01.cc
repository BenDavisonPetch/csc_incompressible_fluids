#include "test_helpers.h"
#include <FVMCode/exceptions.h>
#include <FVMCode/file_parser.h>

#include <cstdlib>
#include <csignal>

// This is a workaround for CTest to implement abort tests, taken from
// https://discourse.cmake.org/t/tests-that-are-meant-to-abort/537
extern "C" void error_test_handle_abort(int) {
    std::_Exit(EXIT_FAILURE);
}

struct test_override_abort {
    test_override_abort() noexcept {
        std::signal(SIGABRT, error_test_handle_abort);
    }
};

test_override_abort handler;

// A test that should fail. Checks that malformed input files cause an abort

int skip_foam_header_01(int, char**)
{
    using namespace FVMCode;

    UnstructuredMesh mesh;
    UnstructuredMeshParser parser (mesh, "mesh_1d/points", "mesh_1d/faces",
                                   "mesh_1d/owner", "mesh_1d/neighbour",
                                   "skip_foam_header_01/malformed");

    return EXIT_SUCCESS;
}