#include "test_helpers.h"
#include <FVMCode/exceptions.h>

void test()
{
    AssertTest(close(2. / 2e-30, 1. / 1e-30));
    AssertTest(close(1. / 3, 1. / 3));
    AssertTest(!close(1. / 3, 1. / 7));
    AssertTest(!close(0, 1. / 7));
    AssertTest(close(0., 0.));
    AssertTest(close(1. - 1., 3. - 3e20/1e20));
}

int main()
{
    test();
    MAIN_OUTPUT;
}