#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <fstream>

#include "point.h"
#include "unstructured_mesh.h"

namespace FVMCode
{

class UnstructuredMeshParser
{
  public:
    UnstructuredMeshParser (UnstructuredMesh  &mesh,
                            const std::string &points_file,
                            const std::string &faces_file,
                            const std::string &cells_file,
                            const std::string &boundary_file);

  private:
    void parse_points ();
    void parse_faces ();
    void parse_cells ();
    void parse_boundaries ();

    // Ensures normal vectors are pointing in the right direction relative to
    // owner and neighbour cells
    void fix_normals ();
    void determine_cell_neighbours();

    UnstructuredMesh &mesh;

    const std::string points_file;
    const std::string faces_file;
    const std::string cells_file;
    const std::string boundary_file;
};

}

#endif