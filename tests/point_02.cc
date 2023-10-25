#include <cmath>
#include <iostream>
#include <sstream>

#include "test_helpers.h"

#include <FVMCode/point.h>

void test ()
{
    using namespace FVMCode;
    
    Point<3, int> p1(4,5,6);
    Point<3, int> p2(1,2,3);
    const Point<3, int> & p1ref = p1;
    const Point<3, int> & p2ref = p2;
    Point<3, int> p3 = p1ref - p2ref;
    AssertTest(p3(0) == 3 && p3(1) == 3 && p3(2) == 3);
}

int main ()
{
    test ();
    MAIN_OUTPUT;

    return EXIT_SUCCESS;
}