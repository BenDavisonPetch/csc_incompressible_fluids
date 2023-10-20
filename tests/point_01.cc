#include <cmath>
#include <iostream>
#include <sstream>

#include "test_helpers.h"

#include <FVMCode/point.h>

void test ()
{
    using namespace FVMCode;
    {
        Point<3, int> p (1, 2, 3);
        Assert (p (0) == 1, "Test failed");
        Assert (p (1) == 2, "Test failed");
        p (2) = 4;
        Assert (p (2) == 4, "Test failed");

        Assert (std::fabs (p.distance (Point<3, int> (1, 2, 4))) < 1e-12,
                "Test failed");

        Point<2> p2 (1, 2);
        Point<2> p3 (3, 5);
        Assert (std::fabs ((p2 + p3).distance (Point<2> (4, 7)) < 1e-12),
                "Test failed");
        Assert (std::fabs ((p2 - p3).distance (Point<2> (-2, -3)) < 1e-12),
                "Test failed");
        Assert (std::fabs ((p2 * 3).distance (Point<2> (3, 6)) < 1e-12),
                "Test failed");
        Assert (std::fabs ((p2 / 2).distance (Point<2> (0.5, 1)) < 1e-12),
                "Test failed");

        Assert (std::fabs (Point<1> (4).distance (Point<1> (2)) - 2) < 1e-12,
                "Test failed");
        Assert (
            std::fabs (Point<3> (0, 4, 1).distance_squared (Point<3> (2, 3, 4))
                       - 14)
                < 1e-12,
            "Test failed");
    }

    {
        const Point<3> p1 (0, 1, 3);
        const Point<3> p2 (1, 4, 2);

        const Point<3> p3 = (p1 + p2) * 2;
        const Point<3> p4 (2, 10, 10);
        AssertTest (close (p3.distance (p4), 0));

        const Point<3> p5 = 3 * (p1 - p2); // 3 * (-1, -3, 1) = (-3, -9, 3)
        Point<3> p6(-3, -9, 3);
        AssertTest(close(p5.distance(p6),0));
    }

    {
        Point<2> p(1., 3.);
        p /= 2; // 0.5, 1.5
        AssertTest(close(p.distance(Point<2>(0.5, 1.5)), 0));
        const Point<2> p2(1.5, 2.5);
        p += Point<2>(1, 1); // 1.5, 2.5
        AssertTest(close(p.distance(p2), 0));

        p -= Point<2>(2, 2); // (-0.5, 0.5)
        p *= 3; // (-1.5, 1.5)
        AssertTest(close(p.distance(Point<2>(-1.5, 1.5)), 0));
    }

    {
        const Point<3, int> p(1, 2, 3);
        std::stringstream ss;
        ss << p;
        AssertTest(ss.str() == "(1, 2, 3)");
    }
}

int main ()
{
    test ();
    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}