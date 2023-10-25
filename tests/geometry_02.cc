#include <cmath>
#include <iostream>
#include <sstream>

#include "FVMCode/point.h"
#include "test_helpers.h"

#include <FVMCode/geometry.h>

void test()
{
    using namespace FVMCode;

    {
        const Point<3> p1(0,0,0);
        const Point<3> p2(1,0,0);
        const Point<3> p3(0,1,0);
        const Point<3> p4(0,0,1);
        std::array<Point<3>, 4> points({p1, p2, p3, p4});
        double volume = Geometry::tetrahedron_volume(points);
        AssertTest(close(volume, 1. / 6));
    }
}

int main()
{
    test();
    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}