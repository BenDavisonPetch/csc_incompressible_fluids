#ifndef MESH_COMPONENTS_H
#define MESH_COMPONENTS_H

#include <algorithm>

#include "exceptions.h"
#include "file_parser_forward.h"
#include "geometry.h"
#include "point.h"

namespace FVMCode
{

/**
 * Represents a generalised face of a dim-dimensional manifold embedded in
 * spacedim-dimensional space. For example, with dim=spacedim=3, this
 * represents a polygonal face connecting two volumetric cells. For dim=2,
 * spacedim=3, this represents a line in 3D space that connects two polygonal
 * "faces" (represented by the Cell class) as part of a surface mesh.
 */
template <int dim, int spacedim = dim> class Face;

/**
 * Represents a generalised cell of a dim-dimensional manifold embedded in a
 * spacedim-dimenisonal space. For example, with dim=spacedim=3, this
 * represents a volumetric cell. For dim=2, spacedim=3 this represents a face
 * of a surface mesh embedded in 3D space.
 */
template <int dim, int spacedim = dim> class Cell;

namespace internal
{
template <int dim, int spacedim = dim>
using CellList = typename std::vector<Cell<dim, spacedim> >;
template <int dim, int spacedim = dim>
using FaceList = typename std::vector<Face<dim, spacedim> >;

template <int dim, int spacedim = dim>
using CellIterator = typename CellList<dim, spacedim>::iterator;
template <int dim, int spacedim = dim>
using FaceIterator = typename FaceList<dim, spacedim>::iterator;
} // namespace internal

template <int dim, int spacedim> class Face
{
  public:
    using PointIterator = internal::PointIterator<spacedim>;
    using CellIterator  = internal::CellIterator<dim, spacedim>;

    Face (const std::vector<PointIterator> &vertices);

    unsigned int                      n_vertices () const;
    const std::vector<PointIterator> &vertices () const;
    /**
     * Returns a container of the iterators of the face's neighbours. The
     * container is guaranteed to have one or two entries, and the first entry
     * is always the cell that owns the face (i.e. the cell for which the
     * normal faces outwards).
     */
    const std::vector<unsigned int> &neighbour_indices () const;

    // TODO: replace return types of below with generalised tensor class
    Point<spacedim>        normal () const;
    double                 area () const;
    Point<spacedim>        area_vector () const;
    const Point<spacedim> &center () const;

    bool         is_boundary () const { return is_boundary_; }
    unsigned int boundary_id () const;

    friend UnstructuredMeshParser;

  private:
    std::vector<PointIterator> vertex_list;
    std::vector<unsigned int>  neighbour_list;
    // TODO: replace with generalised tensor class
    Point<spacedim> area_vec;
    Point<spacedim> centroid;
    double          scalar_area;

    bool is_boundary_;
    int  boundary_id_;
};

template <int dim, int spacedim> class Cell
{
  public:
    using PointIterator = internal::PointIterator<spacedim>;
    using CellIterator  = internal::CellIterator<dim, spacedim>;
    using FaceIterator  = internal::FaceIterator<dim, spacedim>;

    Cell (const std::vector<FaceIterator> &faces);

    /**
     * Returns a container of the iterators of the cells faces.
     */
    const std::vector<FaceIterator> &faces () const;
    /**
     * Returns a container of the cell's neighbours.
     */
    const std::vector<unsigned int> &neighbour_indices () const;

    double                 volume () const { return volume_; }
    const Point<spacedim> &center () const { return centroid; }

    friend UnstructuredMeshParser;

  private:
    std::vector<PointIterator> vertices () const;
    void                       compute_volume_and_centroid ();

    std::vector<FaceIterator> face_list;
    std::vector<unsigned int> neighbour_list;
    Point<spacedim>           centroid;
    double                    volume_;
};

// ======================================
// Implementation
// ======================================

// Face implementation

template <int dim, int spacedim>
Face<dim, spacedim>::Face (const std::vector<PointIterator> &vertices)
    : vertex_list (vertices)
{
    Assert (spacedim >= dim, "Must have spacedim >= dim");
    Assert (vertices.size () >= spacedim,
            "A face must have at least spacedim vertices to be defined!");
    Assert (spacedim == 2 || spacedim == 3, "Not implemented!");

    // Compute area and centroid
    Assert (dim == 3 || dim == 2, "Not implemented");
    if (dim == 3)
    {
        scalar_area
            = Geometry::compute_planar_area_and_centroid (vertices, centroid);
    }
    else if (dim == 2)
    {
        scalar_area = vertices[0]->distance (*vertices[1]);
        centroid    = (*vertices[0] + *vertices[1]) / 2;
    }

    // Compute area vector
    if (dim == 2)
    {
        Assert (vertices.size () == 2,
                "Cannot have a face in 2D with more than two points!");
        // Cannot properly define a normal vector for a 2D face (line) embedded
        // in 3D space
        if (spacedim == 2)
        {
            // TODO: revisit ordering of this
            Point<spacedim> edge = *vertices[1] - *vertices[0];
            area_vec (0)         = edge (1);
            area_vec (1)         = -edge (0);
            area_vec *= scalar_area / area_vec.norm ();
        }
    }
    else if (dim == 3)
    {
        Assert (spacedim == 3, "Not implemented for spacedim > 3");

        const Point<3> edge1 = centroid - *vertices[0];
        const Point<3> edge2 = centroid - *vertices[1];

        Point<3> normalv_tmp = cross_p (edge1, edge2);

        area_vec = normalv_tmp * scalar_area / normalv_tmp.norm ();
    }
}

template <int dim, int spacedim>
inline unsigned int Face<dim, spacedim>::n_vertices () const
{
    return vertex_list.size ();
}

template <int dim, int spacedim>
inline const std::vector<typename Face<dim, spacedim>::PointIterator> &
Face<dim, spacedim>::vertices () const
{
    return vertex_list;
}

template <int dim, int spacedim>
inline const std::vector<unsigned int> &
Face<dim, spacedim>::neighbour_indices () const
{
    return neighbour_list;
}

template <int dim, int spacedim>
inline Point<spacedim> Face<dim, spacedim>::normal () const
{
    Assert (!(dim == 2 && spacedim == 3),
            "Can't define a normal vector for a 2D face (line) embedded in 3D "
            "space.");
    return area_vec / scalar_area;
}

template <int dim, int spacedim>
inline double Face<dim, spacedim>::area () const
{
    return scalar_area;
}

template <int dim, int spacedim>
inline Point<spacedim> Face<dim, spacedim>::area_vector () const
{
    return area_vec;
}

template <int dim, int spacedim>
inline const Point<spacedim> &Face<dim, spacedim>::center () const
{
    return centroid;
}

template <int dim, int spacedim>
inline unsigned int Face<dim, spacedim>::boundary_id () const
{
    Assert (is_boundary_, "Trying to query boundary id of non-boundary cell");
    return boundary_id_;
}

// Cell implementation
template <int dim, int spacedim>
Cell<dim, spacedim>::Cell (const std::vector<FaceIterator> &faces)
    : face_list (faces)
{
    compute_volume_and_centroid ();
}

template <int dim, int spacedim>
inline const std::vector<typename Cell<dim, spacedim>::FaceIterator> &
Cell<dim, spacedim>::faces () const
{
    return face_list;
}

template <int dim, int spacedim>
inline const std::vector<unsigned int> &
Cell<dim, spacedim>::neighbour_indices () const
{
    return neighbour_list;
}

template <int dim, int spacedim>
std::vector<typename Cell<dim, spacedim>::PointIterator>
Cell<dim, spacedim>::vertices () const
{
    std::vector<PointIterator> vertex_list;
    for (const FaceIterator &face : face_list)
    {
        for (const PointIterator &point : face->vertices ())
        {
            if (std::find (vertex_list.begin (), vertex_list.end (), point)
                == vertex_list.end ())
            {
                // point is not contained in vertex_list already
                vertex_list.push_back (point);
            }
        }
    }
    return vertex_list;
}

template <int dim, int spacedim>
void Cell<dim, spacedim>::compute_volume_and_centroid ()
{
    std::vector<PointIterator> vertex_list = vertices ();
    if (dim == 1)
    {
        Assert (vertex_list.size () == 2,
                "Wrong number of vertices for 1D mesh, should only be two");
        volume_  = vertex_list[0]->distance (*vertex_list[1]);
        centroid = (*vertex_list[0] + *vertex_list[1]) / 2;
    }
    else if (dim == 2)
    {
        volume_ = Geometry::compute_planar_area_and_centroid (vertex_list,
                                                              centroid);
    }
    else if (dim == 3)
    {
        Assert (spacedim == 3, "Need spacedim of 3 for a dim 3 mesh");
        // TODO get better code design so this can be split into a separate
        // function

        Point<spacedim> vertex_average;
        for (unsigned int i = 0; i < vertex_list.size (); i++)
        {
            vertex_average += *(vertex_list[i]);
        }
        vertex_average /= vertex_list.size ();

        volume_  = 0;
        centroid = Point<3> (0, 0, 0);

        // loop over faces
        for (const FaceIterator &face : face_list)
        {
            auto face_centroid = face->center ();
            // split face into triangles with centroid
            for (unsigned int i = 0; i < face->n_vertices (); i++)
            {
                std::array<Point<3>, 4> tet_vertices (
                    { vertex_average, face_centroid, *face->vertices ()[i],
                      *face->vertices ()[(i + 1) % face->n_vertices ()] });
                const double tet_volume
                    = Geometry::tetrahedron_volume (tet_vertices);
                volume_ += tet_volume;
                Point<3> tet_centroid
                    = (vertex_average + face_centroid + *face->vertices ()[i]
                       + *face->vertices ()[(i + 1) % face->n_vertices ()])
                      / 4;
                centroid += tet_centroid * tet_volume;
            }
        }
        centroid /= volume_;
    }
    else
    {
        Assert (false, "Not implemented");
    }
}

} // namespace FVMCode

#endif