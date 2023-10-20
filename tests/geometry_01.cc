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
        const Point<2> p1(1,1);
        const Point<2> p2(3,1);
        const Point<2> p3(1, 5);

        AssertTest(close(Geometry::triangle_area(p1,p2,p3), 0.5*2*4));

        std::vector<Point<2>> point_list = {p1, p2, p3};
        std::vector<internal::PointIterator<2>> iters = {point_list.begin(), point_list.begin() + 1, point_list.begin() + 2};
        
        Point<2> centroid;
        const double area = Geometry::compute_planar_area_and_centroid(iters, centroid);
        AssertTest(close(area, 0.5*2*4));
        AssertTest(close(centroid.distance(Point<2>(5. / 3, 7. / 3.)), 0));
    }
    
    {
        const Point<3> p1(1,1, 1);
        const Point<3> p2(1,1, 6);
        const Point<3> p3(1, 7, 1);

        AssertTest(close(Geometry::triangle_area(p1,p2,p3), 0.5*5*6));

        std::vector<Point<3>> point_list = {p1, p2, p3};
        std::vector<internal::PointIterator<3>> iters = {point_list.begin(), point_list.begin() + 1, point_list.begin() + 2};
        
        Point<3> centroid;
        const double area = Geometry::compute_planar_area_and_centroid(iters, centroid);
        AssertTest(close(area, 0.5*5*6));
        AssertTest(close(centroid.distance(Point<3>(1., 3., 8. / 3)), 0));
    }
    
    {
        // Test with regular polygon
        unsigned int n_sides = 5;
        double inner_length = 1.5;
        std::vector<Point<3>> point_list(n_sides);

        const double angle = 2 * M_PIl / n_sides;
        for(unsigned int i = 0; i<n_sides; i++)
        {
            point_list[i] = Point<3>(2., 1. + inner_length*std::cos(angle*i), 0. + inner_length*std::sin(angle*i));
        }

        std::vector<internal::PointIterator<3>> iters(n_sides);
        for(unsigned int i = 0; i<n_sides; i++)
        {
            internal::PointIterator<3> iter = point_list.begin() + i;
            iters[i] = iter;
        }

        

        Point<3> centroid;
        const double area = Geometry::compute_planar_area_and_centroid(iters, centroid);

        AssertTest(close(area, 0.5 * n_sides * inner_length * inner_length * std::sin(angle)));
        AssertTest(close(centroid.distance(Point<3>(2,1,0)), 0));
    }
}

int main()
{
    test();
    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}