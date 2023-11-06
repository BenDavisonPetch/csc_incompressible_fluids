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

    compute_distance_ratios ();
}

UnstructuredMeshParser::UnstructuredMeshParser (
    UnstructuredMesh &mesh, const std::string &points_file,
    const std::string &faces_file, const std::string &owner_file,
    const std::string &neighbour_file, const std::string &boundary_file)
    : mesh (mesh)
{
    parse_points (points_file);
    parse_faces (faces_file);
    parse_owner_neighbour_list (owner_file, neighbour_file);
    parse_boundaries_foam (boundary_file);

    compute_distance_ratios ();
}

void UnstructuredMeshParser::parse_points (const std::string &points_file)
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

void UnstructuredMeshParser::parse_faces (const std::string &faces_file)
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

void UnstructuredMeshParser::parse_cells (const std::string &cells_file)
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

void UnstructuredMeshParser::parse_boundaries_legacy (
    const std::string &boundary_file)
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
        mesh.boundaries.push_back (BoundaryPatch (boundary_name, wall, 0, 0));

        unsigned int n_faces;
        file >> n_faces;

        char open_bracket, closed_bracket;
        file >> open_bracket;

        for (unsigned int f = 0; f < n_faces; f++)
        {
            unsigned int f_index;
            file >> f_index;
            Assert (f_index < mesh.n_faces (), "Face index too high");
        }

        file >> closed_bracket;
        Assert (open_bracket == '(', "Malformed boundary file");
        Assert (closed_bracket == ')', "Malformed boundary file");
    }
}

void UnstructuredMeshParser::parse_owner_neighbour_list (
    const std::string &owner_file, const std::string &neighbour_file)
{
    _add_cells_to_faces_neighbours (owner_file);
    _add_cells_to_faces_neighbours (neighbour_file);

    // Now the faces all have indices in their neighbour lists, and we've
    // populated the map from cell_index -> face_indices
    // We can therefore construct the cells using the faces_of_cell map

    for (const auto &[cell_index, face_indices] : faces_of_cell)
    {
        (void)cell_index;

        std::vector<UnstructuredMesh::FaceIterator> faces (
            face_indices.size ());

        for (unsigned int f = 0; f < face_indices.size (); f++)
        {
            faces[f] = mesh.get_face (face_indices[f]);
        }

        // Note that because faces_of_cell is an ordered map, we expect the
        // cell indices to be in the right order for us to do this:
        Cell<3> cell (faces);
        mesh.cell_list.push_back (cell);
    }
}

// For each line in the owner/neighbour file, add the cell index to the
// neighbour list of the corresponding face. Needs to be called in order using
// parse_owner_neighbour_list.
void UnstructuredMeshParser::_add_cells_to_faces_neighbours (
    const std::string &label_list_file)
{
    std::ifstream file (label_list_file);

    unsigned int n_entries;
    file >> n_entries;

    char temp;
    file >> temp;

    unsigned int cell_index;

    for (unsigned int n = 0; n < n_entries; n++)
    {
        file >> cell_index;
        mesh.get_face (n)->neighbour_list.push_back (cell_index);
        faces_of_cell[cell_index].push_back (n);
    }

    char closed_bracket;
    file >> closed_bracket;
    Assert (closed_bracket == ')', "Malformed owner or neighbour file");
}

void UnstructuredMeshParser::parse_boundaries_foam (
    const std::string &boundary_file)
{
    std::ifstream file (boundary_file);
    unsigned int  n_patches;
    file >> n_patches;

    char open_bracket_round;
    char closed_bracket_round;
    file >> open_bracket_round;
    Assert (open_bracket_round == '(', "Malformed boundary file");

    for (unsigned int n = 0; n < n_patches; n++)
    {
        std::string name;
        file >> name;

        char curly;

        file >> curly;
        Assert (curly == '{', "Malformed boundary file");

        // read in boundary patch details
        std::string  field;
        std::string  type;
        unsigned int n_faces;
        unsigned int start_face;
        while (file >> field)
        {
            if (field == "}")
                break;
            if (field == "type")
            {
                file >> type;
                // Remove semi-colon
                type.pop_back ();
            }
            else if (field == "nFaces")
            {
                file >> n_faces;
                char semicolon;
                file >> semicolon;
                Assert (semicolon == ';', "Malformed boundary file");
            }
            else if (field == "startFace")
            {
                file >> start_face;
                char semicolon;
                file >> semicolon;
                Assert (semicolon == ';', "Malformed boundary file");
            }
            else
            {
                // ignore field
                std::cout << "WARNING: ignoring field " << field << std::endl;
                file >> field;
            }
        }
        // std::cout << "About to add boundary patch..." << std::endl;
        // std::cout << "\tName: " << name << std::endl;
        // std::cout << "\tType: " << type << std::endl;
        // std::cout << "\tn_faces: " << n_faces << std::endl;
        // std::cout << "\tstart_face: " << start_face << std::endl;
        mesh.boundaries.push_back (
            BoundaryPatch (name, type, n_faces, start_face));
    }

    file >> closed_bracket_round;
    Assert (closed_bracket_round == ')', "Malformed boundary file");
}

void UnstructuredMeshParser::fix_normals ()
{
    for (Face<3> &face : mesh.faces ())
    {
        Assert (face.neighbour_list.size () == 1
                    || face.neighbour_list.size () == 2,
                "A face should only have one or two neighbouring cells");
        if (face.neighbour_list.size () == 1)
            // Assert (
            //     face.is_boundary_,
            //     "Face has only one neighbour but is not marked as a
            //     boundary");

            // flip normal vector if required
            if (face.area_vector ().dot (
                    face.center ()
                    - mesh.get_cell (face.neighbour_list[0])->center ())
                < 0)
            {
                // normal vector is facing inwards, so we need to invert
                std::cerr
                    << "WARNING: parser detected normal facing towards "
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

void UnstructuredMeshParser::compute_distance_ratios ()
{
    for (Face<3> &face : mesh.faces()) {
        if(face.is_boundary())
        {
            face.interpolation_factor_ = 1.;
            face.delta_ =  1. / face.center().distance(mesh.get_cell(face.neighbour_list[0])->center());
        } else {
            double cell_center_distance = mesh.get_cell(face.neighbour_list[0])->center().distance(mesh.get_cell(face.neighbour_list[1])->center());
            double face_cell_distance = mesh.get_cell(face.neighbour_list[0])->center().distance(face.center());
            face.delta_ = 1. / cell_center_distance;
            face.interpolation_factor_ = face_cell_distance / cell_center_distance;
        }
    }
}

} // namespace FVMCode