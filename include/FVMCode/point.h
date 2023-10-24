#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <vector>
#include <iostream>

#include "exceptions.h"

namespace FVMCode
{

template <int dim, typename Number = double> class Point;

namespace internal
{
template <int spacedim>
using PointList = typename std::vector<Point<spacedim> >;

template <int spacedim>
using PointIterator = typename PointList<spacedim>::iterator;
}

// Declarations
template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator+(Point<dim, Number> lhs, const Point<dim, OtherNumber>& rhs);
template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator-(Point<dim, Number> lhs, const Point<dim, OtherNumber>& rhs);
template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator*(Point<dim, Number> lhs, const OtherNumber& rhs);
template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator*(OtherNumber lhs, Point<dim, Number> rhs);
template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator/(Point<dim, Number> lhs, const OtherNumber& rhs);
template <int dim, typename Number>
std::ostream& operator<<(std::ostream& os, const Point<dim,Number> &p);

// This has to be templated with dim to compile but is only implemented for dim == 3
template <int dim, typename Number>
inline Point<3, Number> cross_p(const Point<dim, Number> &p1, const Point<dim, Number> &p2);

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

    Point<dim, Number>& operator+= (const Point<dim, Number> &rhs);
    Point<dim, Number>& operator-= (const Point<dim, Number> &rhs);
    Point<dim, Number>& operator*= (const Number &rhs);
    Point<dim, Number>& operator/= (const Number &rhs);

    Number distance_squared (const Point<dim, Number> &p) const;
    Number distance (const Point<dim, Number> &p) const;
    Number norm() const;
    Number norm_squared() const;

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
inline Point<dim, Number>&
Point<dim, Number>::operator+= (const Point<dim, Number> &p)
{
    for (unsigned int i = 0; i < dim; i++)
    {
        values[i] += p(i);
    }
    return *this;
}

template <int dim, typename Number>
inline Point<dim, Number>&
Point<dim, Number>::operator-= (const Point<dim, Number> &p)
{
    for (unsigned int i = 0; i < dim; i++)
    {
        values[i] -= p(i);
    }
    return *this;
}

template <int dim, typename Number>
inline Point<dim, Number>& Point<dim, Number>::operator*= (const Number &a)
{
    for (unsigned int i = 0; i < dim; i++)
    {
        values[i] *= a;
    }
    return *this;
}

template <int dim, typename Number>
inline Point<dim, Number>& Point<dim, Number>::operator/= (const Number &a)
{
    for (unsigned int i = 0; i < dim; i++)
    {
        values[i] /= a;
    }
    return *this;
}

template <int dim, typename Number>
inline Number
Point<dim, Number>::distance_squared (const Point<dim, Number> &p) const
{
    Number sum = 0;
    for (unsigned int i = 0; i < dim; i++)
    {
        sum += std::pow (p (i) - values[i], 2);
    }
    return sum;
}

template <int dim, typename Number>
inline Number Point<dim, Number>::distance (const Point<dim, Number> &p) const
{
    return std::sqrt (distance_squared (p));
}

template <int dim, typename Number>
inline Number Point<dim, Number>::norm () const
{
    return std::sqrt (norm_squared());
}

template <int dim, typename Number>
inline Number Point<dim, Number>::norm_squared () const
{
    Number sum = 0;
    for (unsigned int i = 0; i < dim; i++)
    {
        sum += std::pow (values[i], 2);
    }
    return sum;
}

template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator+(Point<dim, Number> lhs, const Point<dim, OtherNumber>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator-(Point<dim, Number> lhs, const Point<dim, OtherNumber>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator*(Point<dim, Number> lhs, const OtherNumber& rhs)
{
    lhs *= rhs;
    return lhs;
}

template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator*(OtherNumber lhs, Point<dim, Number> rhs)
{
    rhs *= lhs;
    return rhs;
}

template <int dim, typename Number, typename OtherNumber>
Point<dim, Number> operator/(Point<dim, Number> lhs, const OtherNumber& rhs)
{
    lhs /= rhs;
    return lhs;
}

template <int dim, typename Number>
std::ostream& operator<<(std::ostream& os, const Point<dim,Number> &p)
{
    os << "(";
    for (unsigned int i = 0; i < dim; i++)
    {
        os << p(i);
        if (i < dim-1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

template <int dim, typename Number>
inline Point<3, Number> cross_p(const Point<dim, Number> &p1, const Point<dim, Number> &p2)
{
    Assert(dim == 3, "Cross product not defined for dim != 3");
    Point<3, Number> result;
    result(0) = p1(1) * p2(2) - p1(2) * p2(1);
    result(1) = p1(2) * p2(0) - p1(0) * p2(2);
    result(2) = p1(0) * p2(1) - p1(1) * p2(0);
    return result;
}

} // namespace FVMCode

#endif