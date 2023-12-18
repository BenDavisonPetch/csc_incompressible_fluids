#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <fstream>
#include <map>

#include "exceptions.h"
#include "point.h"
#include "unstructured_mesh.h"

namespace FVMCode
{

class UnstructuredMeshParser
{
  public:
    // Reads mesh in format from practical 1
    UnstructuredMeshParser (UnstructuredMesh  &mesh,
                            const std::string &points_file,
                            const std::string &faces_file,
                            const std::string &cells_file,
                            const std::string &boundary_file);

    // Reads mesh in OpenFOAM blockMesh format
    UnstructuredMeshParser (UnstructuredMesh  &mesh,
                            const std::string &points_file,
                            const std::string &faces_file,
                            const std::string &owner_file,
                            const std::string &neighbour_file,
                            const std::string &boundary_file);

    // Reads mesh in OpenFoam format, assuming files are set up as in OpenFoam
    UnstructuredMeshParser (UnstructuredMesh &mesh);

  private:
    std::ifstream skip_foam_header (const std::string &filename) const;
    void          parse_points (const std::string &points_file);
    void          parse_faces (const std::string &faces_file);
    void          parse_cells (const std::string &cells_file);
    void          parse_boundaries_legacy (const std::string &boundary_file);
    void          parse_owner_neighbour_list (const std::string &owner_file,
                                              const std::string &neighbour_file);
    void          parse_boundaries_foam (const std::string &boundary_file);
    void _add_cells_to_faces_neighbours (const std::string &label_list_file);

    // Ensures normal vectors are pointing in the right direction relative to
    // owner and neighbour cells
    void fix_normals ();
    void determine_cell_neighbours ();

    void compute_distance_ratios ();

    UnstructuredMesh &mesh;

    std::map<unsigned int, std::vector<unsigned int> > faces_of_cell;
};

}

#endif