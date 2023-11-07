#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

void output_results(const VectorXd &v, unsigned int output_counter);

void run()
{
    using namespace FVMCode;
    UnstructuredMesh mesh;
    UnstructuredMeshParser parser(mesh, "scripts/transient_laplacian/constant/polyMesh/points", "scripts/transient_laplacian/constant/polyMesh/faces", "scripts/transient_laplacian/constant/polyMesh/owner", "scripts/transient_laplacian/constant/polyMesh/neighbour", "scripts/transient_laplacian/constant/polyMesh/boundary");

    // Setup system
    MatrixXd system_matrix(mesh.n_cells(), mesh.n_cells());
    VectorXd system_rhs(mesh.n_cells());
    VectorXd temperature(mesh.n_cells());

    // Initial conditions
    temperature = VectorXd::Constant(mesh.n_cells(), 1);
    unsigned int output_counter = 0;
    output_results(temperature, output_counter);
    output_counter++;

    const double dt = 0.05;
    const double diff_const = 1.;

    double time = 0;
    const double end_time = 1;

    while (time < end_time)
    {
        time += dt;

        system_matrix = MatrixXd::Zero(mesh.n_cells(), mesh.n_cells());
        system_rhs = VectorXd::Zero(mesh.n_cells());
        // temporal term
        for (unsigned int i = 0; i < mesh.n_cells(); i++)
        {
            const double volume = mesh.get_cell(i)->volume();
            system_matrix(i,i) = volume / dt;
            system_rhs(i) = system_matrix(i,i) * temperature(i);
        }

        // diffusion term (internal cells)
        for (unsigned int f = 0; f < mesh.n_faces(); f++)
        {
            const auto& face = mesh.get_face(f);
            if (face->is_boundary()) continue;
            const unsigned int owner_index = face->neighbour_indices()[0];
            const unsigned int neighbour_index = face->neighbour_indices()[1];
            const double a_N = diff_const * face->area() * face->delta();
            system_matrix(owner_index, owner_index) += a_N;
            system_matrix(neighbour_index, owner_index) = -a_N;
        }

        // diffusion term (boundary conditions)
        for (const auto& boundary : mesh.get_patches())
        {
            if(boundary.type == empty) continue;

            for (unsigned int f = boundary.start_face; f < boundary.start_face + boundary.n_faces; f++)
            {
                const double boundary_val = 0;
                const auto& face = mesh.get_face(f);
                const unsigned int owner_index = face->neighbour_indices()[0];
                const double a_N = diff_const * face->area() * face->delta();
                system_matrix(owner_index, owner_index) += a_N;
                system_rhs(owner_index) += a_N * boundary_val;
            }
        }

        // solve system
        temperature = system_matrix.colPivHouseholderQr().solve(system_rhs);

        // output temperature
        output_results(temperature, output_counter);
        output_counter++;
    }
}

void output_results(const VectorXd &v, unsigned int output_counter)
{
    // std::stringstream ss;
    // std::string       counter_str = std::to_string (output_counter);
    // unsigned long     num_zeros   = 5;
    // ss << std::string (num_zeros - std::min (num_zeros, counter_str.length ()),
    //                    '0')
    //    << counter_str << ".dat";
    // std::ofstream output (ss.str ());
    std::cout << v << std::endl << std::endl;
}

int main()
{
    run();
    return EXIT_SUCCESS;
}