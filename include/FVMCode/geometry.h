#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "point.h"

namespace FVMCode
{
namespace Geometry
{

/**
 * Computes the area and centroid of a planar 2D object embedded in spacedim
 * dimensions. The list of vertices must be in cyclic order. Returns the area
 * as a double, and replaces @param centroid with the centroid.
 */
template <int spacedim>
double compute_planar_area_and_centroid (
    const std::vector<internal::PointIterator<spacedim> > &vertices,
    Point<spacedim>                                       &centroid);

/**
 * Computes the area of a triangle
 */
template <int spacedim>
double triangle_area (const Point<spacedim> &p1, const Point<spacedim> &p2,
                      const Point<spacedim> &p3);

// =============================
// Implementation
// ============================

template <int spacedim>
double compute_planar_area_and_centroid (
    const std::vector<internal::PointIterator<spacedim> > &vertices,
    Point<spacedim>                                       &centroid)
{
    Assert (spacedim == 3 || spacedim == 2, "Not implemented");
    Assert (vertices.size () >= 3,
            "Cannot form a valid planar object with less than 3 points");
    // TODO: add a check that vertices are all planar in 2D, colinear in 1D,
    // etc (applicable when spacedim > dim)

    Point<spacedim> vertex_average;
    for (unsigned int i = 0; i < vertices.size (); i++)
    {
        vertex_average += *(vertices[i]);
    }
    vertex_average /= vertices.size ();

    // A triangle's centroid is just the average of its vertices
    if (vertices.size () == 3)
    {
        centroid = vertex_average;
        return triangle_area (*vertices[0], *vertices[1], *vertices[2]);
    }

    // If the shape is not a triangle, the centroid is the area weighted
    // average of the centroids of the triangles that the shape can be split up
    // into from the vertices to the average vertex position.
    // Here we assume that the vertex labels come in the order one would
    // expected them to be connected if the shape was drawn out
    Point<spacedim> area_weighted_sum;
    double          total_area = 0;
    for (unsigned int i = 0; i < vertices.size (); i++)
    {
        double area = triangle_area (vertex_average, *vertices[i],
                                     *vertices[(i + 1) % vertices.size ()]);
        area_weighted_sum += area
                             * (vertex_average + *vertices[i]
                                + *vertices[(i + 1) % vertices.size ()])
                             / 3;
        total_area += area;
    }
    centroid = area_weighted_sum / total_area;
    return total_area;
}

template <int spacedim>
double triangle_area (const Point<spacedim> &p1, const Point<spacedim> &p2,
                      const Point<spacedim> &p3)
{
    Assert (spacedim == 2 || spacedim == 3, "Not implemented");

    Point<spacedim> edge1 = p2 - p1;
    Point<spacedim> edge2 = p3 - p1;

    if (spacedim == 3)
    {
        return 0.5 * cross_p (edge1, edge2).distance (Point<3> (0, 0, 0));
    }
    else if (spacedim == 2)
    {
        return 0.5 * (edge1 (0) * edge2 (1) - edge1 (1) * edge2 (0));
    }
    return 0;
}

} // namespace FVMCode::Geometry
} // namespace FVMCode

#endif