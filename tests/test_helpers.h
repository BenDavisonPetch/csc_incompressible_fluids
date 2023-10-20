#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <iostream>

#define AssertTest(cond) Assert(cond, "Test failed");

#ifndef DEBUG
#    define MAIN_OUTPUT \
    std::cout << "WARNING: Debug is not enabled, so Asserts are not active!" \
              << std::endl;
#else
#    define MAIN_OUTPUT std::cout << "Success!" << std::endl;
#endif

#include <cmath>

inline bool close(double a, double b)
{
    if (a == 0) return b < 1e-12;
    if (b == 0) return a < 1e-12;
    return std::fabs(a-b) <= std::fmax(std::fabs(a)*1e-12, std::fabs(b)*1e-12);
}

#endif