#include "test_helpers.h"
#include <FVMCode/exceptions.h>

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

// A test that should fail. Checks that the testing system is working correctly

int assert_test_01(int, char**)
{
    AssertTest(false);

    return EXIT_SUCCESS;
}