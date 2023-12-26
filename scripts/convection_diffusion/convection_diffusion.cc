#include <FVMCode/boundary_patch.h>
#include <FVMCode/exceptions.h>
#include <FVMCode/file_parser.h>
#include <FVMCode/output.h>
#include <FVMCode/unstructured_mesh.h>

#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::Vector3d;
using Eigen::VectorXd;

using namespace FVMCode;
using BoundaryConditions
    = std::vector<std::pair<BoundaryPatch, BoundaryFieldEntry> >;

void output (const VectorXd &temperature, const BoundaryConditions &bcs,
             double &next_output_time, const unsigned int &timestep_number,
             const double &time, const double &dt,
             const double &output_time_interval);

void construct_temporal_term (MatrixXd &system_matrix, VectorXd &system_rhs,
                              UnstructuredMesh &mesh,
                              const VectorXd &temperature, const double dt);

void construct_source (VectorXd &system_rhs, UnstructuredMesh &mesh,
                       unsigned int source_cell_index, double source_strength);

void construct_diffusion_term (MatrixXd &system_matrix, VectorXd &system_rhs,
                               UnstructuredMesh         &mesh,
                               const BoundaryConditions &bcs,
                               const double              diffusion_const);

void construct_convection_term_centred (MatrixXd         &system_matrix,
                                        VectorXd         &system_rhs,
                                        UnstructuredMesh &mesh,
                                        const BoundaryConditions &bcs,
                                        const Point<3, double>   &velocity);

void construct_convection_term_upwind (MatrixXd         &system_matrix,
                                        VectorXd         &system_rhs,
                                        UnstructuredMesh &mesh,
                                        const BoundaryConditions &bcs,
                                        const Point<3, double>   &velocity);

void construct_convection_term_BCs (MatrixXd                 &system_matrix,
                                    VectorXd                 &system_rhs,
                                    UnstructuredMesh         &mesh,
                                    const BoundaryConditions &bcs,
                                    const Point<3, double>   &velocity);

int main ()
{
    UnstructuredMesh mesh;
    {
        UnstructuredMeshParser parser (mesh);
    }

    std::cout << "Mesh parsed" << std::endl;

    // BCs
    BoundaryConditions bc;

    std::vector<BoundaryPatch> patches = mesh.get_patches ();
    for (unsigned int p = 0; p < mesh.n_boundary_patches (); p++)
    {
        auto patch = patches[p];
        if (patch.type == empty)
        {
            bc.push_back (std::make_pair<BoundaryPatch, BoundaryFieldEntry> (
                BoundaryPatch (patch), BoundaryFieldEntry ("empty", 0)));
            continue;
        }
        else if (patch.name == "leftWall")
        {
            bc.push_back (std::make_pair<BoundaryPatch, BoundaryFieldEntry> (
                BoundaryPatch (patch), BoundaryFieldEntry ("fixedValue", 0)));
            continue;
        }

        bc.push_back (std::make_pair<BoundaryPatch, BoundaryFieldEntry> (
            BoundaryPatch (patch), BoundaryFieldEntry ("zeroGradient", 0)));
    }

    std::cout << "BCs constructed" << std::endl;

    // Setup
    MatrixXd system_matrix (mesh.n_cells (), mesh.n_cells ());
    VectorXd system_rhs (mesh.n_cells ());
    VectorXd temperature (mesh.n_cells ());

    const double   dt                   = 0.05;
    const double   output_time_interval = 0.05;
    const double   diffusion_const      = 0.01;
    const double   source_strength      = 1.;
    const Point<3> source_location (0.5, 0., 0.);
    const Point<3> velocity (1., 0., 0.);

    unsigned int source_cell_index
        = mesh.get_cell_containing_point (source_location);

    double       time     = 0;
    const double end_time = 1.;

    std::cout << "System setup" << std::endl;

    // First output
    unsigned int timestep_number  = 0;
    double       next_output_time = time;
    output (temperature, bc, next_output_time, timestep_number, time, dt,
            output_time_interval);

    std::cout << "First output complete" << std::endl;

    while (time < end_time)
    {
        time += dt;
        timestep_number++;
        std::cout << std::endl
                  << "Starting timestep " << timestep_number
                  << ", t = " << time << std::endl
                  << std::endl;

        system_matrix = MatrixXd::Zero (mesh.n_cells (), mesh.n_cells ());
        system_rhs    = VectorXd::Zero (mesh.n_cells ());

        construct_temporal_term (system_matrix, system_rhs, mesh, temperature,
                                 dt);
        construct_source (system_rhs, mesh, source_cell_index,
                          source_strength);
        construct_diffusion_term (system_matrix, system_rhs, mesh, bc,
                                  diffusion_const);
        construct_convection_term_upwind (system_matrix, system_rhs, mesh, bc,
                                           velocity);

        std::cout << "\tSystem assembled" << std::endl;

        // solve system
        temperature = system_matrix.colPivHouseholderQr ().solve (system_rhs);
        std::cout << "\tSystem solved" << std::endl;
        output (temperature, bc, next_output_time, timestep_number, time, dt,
                output_time_interval);
        std::cout << "\tOutput complete" << std::endl;
    }
}

void output (const VectorXd &temperature, const BoundaryConditions &bcs,
             double &next_output_time, const unsigned int &timestep_number,
             const double &time, const double &dt,
             const double &output_time_interval)
{
    if (time >= next_output_time - 1e-12)
    {
        std::cout << "\tOUTPUTTING" << std::endl;
        next_output_time = time + output_time_interval;
        std::cout << "\tNext output time = " << next_output_time << std::endl;

        Outputter outputter (timestep_number, time, dt, dt);
        outputter.write_time ();
        outputter.write_scalar_field (temperature, "T", bcs);
    }
}

void construct_temporal_term (MatrixXd &system_matrix, VectorXd &system_rhs,
                              UnstructuredMesh &mesh,
                              const VectorXd &temperature, const double dt)
{
    for (unsigned int i = 0; i < mesh.n_cells (); i++)
    {
        const double volume = mesh.get_cell (i)->volume ();
        system_matrix (i, i) += volume / dt;
        system_rhs (i) += volume / dt * temperature (i);
    }
}

// Describes a source term of strength source_strength[T]/s in cell specified
void construct_source (VectorXd &system_rhs, UnstructuredMesh &mesh,
                       unsigned int source_cell_index, double source_strength)
{
    system_rhs (source_cell_index)
        += source_strength * mesh.get_cell (source_cell_index)->volume ();
}

void construct_diffusion_term (MatrixXd &system_matrix, VectorXd &system_rhs,
                               UnstructuredMesh         &mesh,
                               const BoundaryConditions &bcs,
                               const double              diffusion_const)
{
    // Internal cells
    for (unsigned int f = 0; f < mesh.get_patches ()[0].start_face; f++)
    {
        const auto &face = mesh.get_face (f);
        Assert (!face->is_boundary (),
                "Face should not be at boundary! Check face numbering.");

        const unsigned int owner_index     = face->neighbour_indices ()[0];
        const unsigned int neighbour_index = face->neighbour_indices ()[1];
        const double a_N = diffusion_const * face->area () * face->delta ();
        system_matrix (owner_index, owner_index) += a_N;
        system_matrix (neighbour_index, neighbour_index) += a_N;
        system_matrix (owner_index, neighbour_index) += -a_N;
        system_matrix (neighbour_index, owner_index) += -a_N;
    }

    // Boundary cells
    for (unsigned int n_patch = 0; n_patch < mesh.n_boundary_patches ();
         n_patch++)
    {
        const auto &boundary = mesh.get_patches ()[n_patch];
        if (boundary.type == empty)
            break;

        const auto &b_field = bcs[n_patch].second;
        Assert (b_field.type != "empty",
                "Type of BoundaryPatch and BoundaryFieldEntry do not match");

        if (b_field.type == "fixedValue")
            // Dirichlet BCs
            for (unsigned int f = boundary.start_face;
                 f < boundary.start_face + boundary.n_faces; f++)
            {
                const auto        &face        = mesh.get_face (f);
                const unsigned int owner_index = face->neighbour_indices ()[0];
                const double       a_N
                    = diffusion_const * face->area () * face->delta ();
                system_matrix (owner_index, owner_index) += a_N;
                system_rhs (owner_index) += a_N * b_field.value;
            }
        else if (b_field.type == "zeroGradient")
        {
            // Homogenous Neumann BCs
            continue;
        }
        else
        {
            Assert (false, "Boundary type not implemented");
        }
    }
}

void construct_convection_term_centred (MatrixXd         &system_matrix,
                                        VectorXd         &system_rhs,
                                        UnstructuredMesh &mesh,
                                        const BoundaryConditions &bcs,
                                        const Point<3, double>   &velocity)
{

    // Internal cells
    for (unsigned int f = 0; f < mesh.get_patches ()[0].start_face; f++)
    {
        const auto &face = mesh.get_face (f);
        Assert (!face->is_boundary (),
                "Face should not be at boundary! Check face numbering.");

        const unsigned int owner_index     = face->neighbour_indices ()[0];
        const unsigned int neighbour_index = face->neighbour_indices ()[1];
        const double       face_flux = velocity.dot (face->area_vector ());

        system_matrix (owner_index, owner_index)
            += face_flux * face->interpolation_factor ();
        system_matrix (neighbour_index, neighbour_index)
            += -face_flux * (1 - face->interpolation_factor ());
        system_matrix (owner_index, neighbour_index)
            += face_flux * (1 - face->interpolation_factor ());
        system_matrix (neighbour_index, owner_index)
            += -face_flux * face->interpolation_factor ();
    }

    construct_convection_term_BCs (system_matrix, system_rhs, mesh, bcs,
                                   velocity);
}

void construct_convection_term_upwind (MatrixXd         &system_matrix,
                                        VectorXd         &system_rhs,
                                        UnstructuredMesh &mesh,
                                        const BoundaryConditions &bcs,
                                        const Point<3, double>   &velocity)
{

    // Internal cells
    for (unsigned int f = 0; f < mesh.get_patches ()[0].start_face; f++)
    {
        const auto &face = mesh.get_face (f);
        Assert (!face->is_boundary (),
                "Face should not be at boundary! Check face numbering.");

        const unsigned int owner_index     = face->neighbour_indices ()[0];
        const unsigned int neighbour_index = face->neighbour_indices ()[1];
        const double       face_flux = velocity.dot (face->area_vector ());

        if (face_flux > 0.)
        {
            system_matrix(owner_index, owner_index) += face_flux;
            system_matrix(neighbour_index, owner_index) += -face_flux;
        } else {
            system_matrix(neighbour_index, neighbour_index) += -face_flux;
            system_matrix(owner_index, neighbour_index) += face_flux;
        }
    }

    construct_convection_term_BCs (system_matrix, system_rhs, mesh, bcs,
                                   velocity);
}

void construct_convection_term_BCs (MatrixXd                 &system_matrix,
                                    VectorXd                 &system_rhs,
                                    UnstructuredMesh         &mesh,
                                    const BoundaryConditions &bcs,
                                    const Point<3, double>   &velocity)
{
    // Boundary cells
    for (unsigned int n_patch = 0; n_patch < mesh.n_boundary_patches ();
         n_patch++)
    {
        const auto &boundary = mesh.get_patches ()[n_patch];
        if (boundary.type == empty)
            break;

        const auto &b_field = bcs[n_patch].second;
        Assert (b_field.type != "empty",
                "Type of BoundaryPatch and BoundaryFieldEntry do not match");

        if (b_field.type == "fixedValue")
            // Dirichlet BCs
            for (unsigned int f = boundary.start_face;
                 f < boundary.start_face + boundary.n_faces; f++)
            {
                const auto        &face        = mesh.get_face (f);
                const unsigned int owner_index = face->neighbour_indices ()[0];
                const double face_flux = velocity.dot (face->area_vector ());
                system_rhs (owner_index) += -face_flux * b_field.value;
            }
        else if (b_field.type == "zeroGradient")
        {
            // Homogenous Neumann BCs
            for (unsigned int f = boundary.start_face;
                 f < boundary.start_face + boundary.n_faces; f++)
            {
                const auto        &face        = mesh.get_face (f);
                const unsigned int owner_index = face->neighbour_indices ()[0];
                const double face_flux = velocity.dot (face->area_vector ());
                system_matrix (owner_index, owner_index) += face_flux;
            }
        }
        else
        {
            Assert (false, "Boundary type not implemented");
        }
    }
}