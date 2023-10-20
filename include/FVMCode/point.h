#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <vector>

#include "exceptions.h"

namespace FVMCode
{

template <int dim, typename Number = double> class Point;

namespace internal
{
template <int spacedim>
using PointList = typename std::vector<Point<spacedim>>;

template <int spacedim>
using PointIterator = typename PointList<spacedim>::iterator;
}

template <int dim, typename Number> class Point
{
  public:
    // Creates a point with Number's default constructor
    Point ();

    Point (const Number &x);
    Point (const Number &x, const Number &y);
    Point (const Number &x, const Number &y, const Number &z);

    Number &operator() (std::size_t idx);
    Number  operator() (std::size_t idx) const;

    Point<dim, Number> operator+ (const Point<dim, Number> &p);
    Point<dim, Number> operator- (const Point<dim, Number> &p);
    Point<dim, Number> operator* (const Number &a);
    Point<dim, Number> operator/ (const Number &a);

    Number distance_squared (const Point<dim, Number> &p);
    Number distance (const Point<dim, Number> &p);

  private:
    std::vector<Number> values;
};

} // namespace FVMCode

// Implementations

namespace FVMCode
{

template <int dim, typename Number>
Point<dim, Number>::Point ()
    : values (dim)
{
}

template <int dim, typename Number>
Point<dim, Number>::Point (const Number &x)
    : values (1)
{
    static_assert (
        dim == 1,
        "This constructor can only be used for points with dim == 1");
    values[0] = x;
}

template <int dim, typename Number>
Point<dim, Number>::Point (const Number &x, const Number &y)
    : values (2)
{
    static_assert (
        dim == 2,
        "This constructor can only be used for points with dim == 2");
    values[0] = x;
    values[1] = y;
}

template <int dim, typename Number>
Point<dim, Number>::Point (const Number &x, const Number &y, const Number &z)
    : values (3)
{
    static_assert (
        dim == 3,
        "This constructor can only be used for points with dim == 3");
    values[0] = x;
    values[1] = y;
    values[2] = z;
}

template <int dim, typename Number>
inline Number &Point<dim, Number>::operator() (std::size_t idx)
{
    AssertIndexRange (idx, dim);
    return values[idx];
}
template <int dim, typename Number>
inline Number Point<dim, Number>::operator() (std::size_t idx) const
{
    AssertIndexRange (idx, dim);
    return values[idx];
}

template <int dim, typename Number>
inline Point<dim, Number>
Point<dim, Number>::operator+ (const Point<dim, Number> &p)
{
    Point<dim, Number> new_p;
    for (unsigned int i = 0; i < dim; i++)
    {
        new_p (i) = p (i) + values[i];
    }
    return new_p;
}

template <int dim, typename Number>
inline Point<dim, Number>
Point<dim, Number>::operator- (const Point<dim, Number> &p)
{
    Point<dim, Number> new_p;
    for (unsigned int i = 0; i < dim; i++)
    {
        new_p (i) = values[i] - p (i);
    }
    return new_p;
}

template <int dim, typename Number>
inline Point<dim, Number> Point<dim, Number>::operator* (const Number &a)
{
    Point<dim, Number> new_p;
    for (unsigned int i = 0; i < dim; i++)
    {
        new_p (i) = values[i] * a;
    }
    return new_p;
}

template <int dim, typename Number>
inline Point<dim, Number> Point<dim, Number>::operator/ (const Number &a)
{
    Point<dim, Number> new_p;
    for (unsigned int i = 0; i < dim; i++)
    {
        new_p (i) = values[i] / a;
    }
    return new_p;
}

template <int dim, typename Number>
inline Number
Point<dim, Number>::distance_squared (const Point<dim, Number> &p)
{
    Number sum = 0;
    for (unsigned int i = 0; i < dim; i++)
    {
        sum += std::pow (p (i) - values[i], 2);
    }
    return sum;
}

template <int dim, typename Number>
inline Number Point<dim, Number>::distance (const Point<dim, Number> &p)
{
    return std::sqrt (distance_squared (p));
}

} // namespace FVMCode

#endif