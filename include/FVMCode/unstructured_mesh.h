#ifndef UNSTRUCTURED_MESH_H
#define UNSTRUCTURED_MESH_H

#include <vector>
#include <iterator>

#include "file_parser_forward.h"
#include "mesh_components.h"
#include "point.h"

namespace FVMCode
{
class UnstructuredMesh
{
  public:
    using PointList = internal::PointList<3>;
    using CellList  = internal::CellList<3>;
    using FaceList  = internal::FaceList<3>;

    using PointIterator = PointList::iterator;
    using CellIterator  = CellList::iterator;
    using FaceIterator  = FaceList::iterator;

    unsigned int n_points () const;
    unsigned int n_faces () const;
    unsigned int n_cells () const;
    unsigned int n_boundary_patches () const;

    // TODO work out a way to provide cell_iterators and face_iterators
    // functions that provide write access to the underlying cells and faces
    // without granting access to cell_list and point_list
    CellList &cells ();
    FaceList &faces ();

    PointIterator get_point (unsigned int index);
    CellIterator  get_cell (unsigned int index);
    FaceIterator  get_face (unsigned int index);

    friend UnstructuredMeshParser;

  private:
    PointList                point_list;
    CellList                 cell_list;
    FaceList                 face_list;
    std::vector<std::string> boundary_names;
};

} // namespace FVMCode

// ==========================
//     Implementations
// ==========================

namespace FVMCode {

inline unsigned int UnstructuredMesh::n_points() const { return point_list.size(); }
inline unsigned int UnstructuredMesh::n_faces() const { return face_list.size(); }
inline unsigned int UnstructuredMesh::n_cells() const { return cell_list.size(); }
inline unsigned int UnstructuredMesh::n_boundary_patches() const { return boundary_names.size(); }

inline UnstructuredMesh::CellList& UnstructuredMesh::cells() {
    return cell_list;
}

inline UnstructuredMesh::FaceList& UnstructuredMesh::faces() {
    return face_list;
}

inline UnstructuredMesh::PointIterator UnstructuredMesh::get_point(unsigned int index)
{
    Assert(index < n_points(), "List index out of range");
    return point_list.begin() + index;
}

inline UnstructuredMesh::CellIterator UnstructuredMesh::get_cell(unsigned int index)
{
    Assert(index < cell_list.size(), "List index out of range");
    return cell_list.begin() + index;
}

inline UnstructuredMesh::FaceIterator UnstructuredMesh::get_face(unsigned int index)
{
    Assert(index < n_faces(), "List index out of range");
    return face_list.begin() + index;
}

} // namespace FVMCode

#endif