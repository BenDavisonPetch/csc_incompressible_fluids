#include <FVMCode/file_parser.h>

namespace FVMCode
{

UnstructuredMeshParser::UnstructuredMeshParser (
    UnstructuredMesh &mesh, const std::string &points_file,
    const std::string &faces_file, const std::string &cells_file,
    const std::string &boundary_file)
    : mesh (mesh)
{
    parse_points (points_file);
    parse_faces (faces_file);
    parse_cells (cells_file);
    parse_boundaries_legacy (boundary_file);

    fix_normals ();
    determine_cell_neighbours ();
}

UnstructuredMeshParser::UnstructuredMeshParser (
    UnstructuredMesh &mesh, const std::string &points_file,
    const std::string &faces_file, const std::string &owner_file,
    const std::string &neighbour_file, const std::string &boundary_file)
: mesh(mesh)
{
    
}

void UnstructuredMeshParser::parse_points (const std::string& points_file)
{
    std::ifstream file (points_file);
    unsigned int  n_points;
    file >> n_points;
    Assert (n_points, "Must have at least one point");

    mesh.point_list.resize (n_points);

    // Read first open parenthesis
    char temp;
    file >> temp;

    double x, y, z;
    for (unsigned int n = 0; n < n_points; n++)
    {
        char open_bracket, closed_bracket;
        file >> open_bracket >> x >> y >> z >> closed_bracket;
        Assert (open_bracket == '(', "Malformed point file");
        Assert (closed_bracket == ')', "Malformed point file");
        mesh.point_list[n] = Point<3> (x, y, z);
    }
}

void UnstructuredMeshParser::parse_faces (const std::string& faces_file)
{
    std::ifstream file (faces_file);
    unsigned int  n_faces;
    file >> n_faces;
    Assert (n_faces, "Must have at least one face");

    // Read first open parenthesis
    char temp;
    file >> temp;

    for (unsigned int n = 0; n < n_faces; n++)
    {
        // Read in number of points on face
        unsigned int n_vertices;
        file >> n_vertices;
        Assert (n_vertices, "Malformed face file");

        char open_bracket, closed_bracket;
        file >> open_bracket;

        std::vector<UnstructuredMesh::PointIterator> vertices (n_vertices);

        for (unsigned int v = 0; v < n_vertices; v++)
        {
            unsigned int index;
            file >> index;
            Assert (index < mesh.n_points (), "Point index too large");
            vertices[v] = mesh.get_point (index);
        }

        file >> closed_bracket;
        Assert (open_bracket == '(', "Malformed point file");
        Assert (closed_bracket == ')', "Malformed point file");

        // Reading in of face has been successful so we can construct the Face
        // object and give it to the mesh
        mesh.face_list.push_back (Face<3> (vertices));
    }
}

void UnstructuredMeshParser::parse_cells (const std::string& cells_file)
{
    std::ifstream file (cells_file);
    unsigned int  n_cells;
    file >> n_cells;
    Assert (n_cells, "Must have at least one cell");

    // Read first open parenthesis
    char temp;
    file >> temp;

    for (unsigned int n = 0; n < n_cells; n++)
    {
        // Read in number of points on face
        unsigned int n_faces;
        file >> n_faces;
        Assert (n_faces, "Malformed face file");

        char open_bracket, closed_bracket;
        file >> open_bracket;

        std::vector<UnstructuredMesh::FaceIterator> faces (n_faces);

        for (unsigned int f = 0; f < n_faces; f++)
        {
            unsigned int index;
            file >> index;
            Assert (index < mesh.n_faces (), "Face index too large");
            faces[f] = mesh.get_face (index);
        }

        file >> closed_bracket;
        Assert (open_bracket == '(', "Malformed point file");
        Assert (closed_bracket == ')', "Malformed point file");

        // Reading in of cell has been successful so we can construct the Cell
        // object and give it to the mesh
        Cell<3> cell (faces);
        mesh.cell_list.push_back (cell);

        // Now we add the cell to the neighbour list of each face in its list
        // The order of owner and neighbour will be fixed later on by
        // fix_normals()
        for (UnstructuredMesh::FaceIterator face : faces)
        {
            face->neighbour_list.push_back (n);
        }
    }
}

void UnstructuredMeshParser::parse_boundaries_legacy (const std::string& boundary_file)
{
    std::ifstream file (boundary_file);
    unsigned int  n_boundary_types;
    file >> n_boundary_types;
    Assert (n_boundary_types, "Must have at least one boundary type");

    // Read first open parenthesis
    char temp;
    file >> temp;

    for (unsigned int b = 0; b < n_boundary_types; b++)
    {
        std::string boundary_name;
        file >> boundary_name;
        mesh.boundary_names.push_back (boundary_name);

        unsigned int n_faces;
        file >> n_faces;

        char open_bracket, closed_bracket;
        file >> open_bracket;

        for (unsigned int f = 0; f < n_faces; f++)
        {
            unsigned int f_index;
            file >> f_index;
            Assert (f_index < mesh.n_faces (), "Face index too high");
            mesh.face_list[f_index].is_boundary_ = true;
            mesh.face_list[f_index].boundary_id_ = b;
        }

        file >> closed_bracket;
        Assert (open_bracket == '(', "Malformed boundary file");
        Assert (closed_bracket == ')', "Malformed boundary file");
    }
}

void UnstructuredMeshParser::fix_normals ()
{
    for (Face<3> &face : mesh.faces ())
    {
        Assert (face.neighbour_list.size () == 1
                    || face.neighbour_list.size () == 2,
                "A face should only have one or two neighbouring cells");
        if (face.neighbour_list.size () == 1)
            Assert (
                face.is_boundary_,
                "Face has only one neighbour but is not marked as a boundary");

        // flip normal vector if required
        if (face.area_vector ().dot (
                face.center ()
                - mesh.get_cell (face.neighbour_list[0])->center ())
            < 0)
        {
            // normal vector is facing inwards, so we need to invert
            std::cerr << "WARNING: parser detected normal facing towards "
                         "owner cell centroid. Ensure vertices of faces are "
                         "given in a right handed manner, and that the first "
                         "cell that mentions a face's index is its owner."
                      << std::endl;
            face.area_vec *= -1;
        }
    }
}

void UnstructuredMeshParser::determine_cell_neighbours ()
{
    for (const Face<3> &face : mesh.faces ())
    {
        if (face.is_boundary ())
            continue;

        mesh.get_cell (face.neighbour_indices ()[0])
            ->neighbour_list.push_back (face.neighbour_indices ()[1]);
        mesh.get_cell (face.neighbour_indices ()[1])
            ->neighbour_list.push_back (face.neighbour_indices ()[0]);
    }
}

} // namespace FVMCode