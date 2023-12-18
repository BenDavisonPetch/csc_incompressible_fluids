#include "test_helpers.h"
#include <FVMCode/exceptions.h>

#include <cstdlib>
#include <csignal>

// A test that should fail. Checks that the testing system is working correctly

int assert_test_01(int, char**)
{
    AssertTest(false);

    return EXIT_SUCCESS;
}