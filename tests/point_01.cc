#include <iostream>
#include <cmath>

#include <FVMCode/point.h>

void test()
{
    using namespace FVMCode;
    {
        Point<3, int> p(1, 2, 3);
        Assert(p(0) == 1, "Test failed");
        Assert(p(1) == 2, "Test failed");
        p(2) = 4;
        Assert(p(2) == 4, "Test failed");

        Assert(std::fabs(p.distance(Point<3, int>(1, 2, 4))) < 1e-12, "Test failed");

        Point<2> p2(1, 2);
        Point<2> p3 (3, 5);
        Assert(std::fabs((p2+p3).distance(Point<2>(4, 7)) < 1e-12), "Test failed");
        Assert(std::fabs((p2-p3).distance(Point<2>(-2, -3)) < 1e-12), "Test failed");
        Assert(std::fabs((p2*3).distance(Point<2>(3, 6)) < 1e-12), "Test failed");
        Assert(std::fabs((p2/2).distance(Point<2>(0.5, 1)) < 1e-12), "Test failed");

        Assert(std::fabs(Point<1>(4).distance(Point<1>(2)) - 2) < 1e-12, "Test failed");
        Assert(std::fabs(Point<3>(0,4,1).distance_squared(Point<3>(2,3,4)) - 14) < 1e-12, "Test failed");
    }   
}

int main()
{
    test();
#ifndef DEBUG
    std::cout << "WARNING: Debug is not enabled, so Asserts are not active!" << std::endl;
#else
    std::cout << "Success!" << std::endl;
#endif

    return EXIT_SUCCESS;
}