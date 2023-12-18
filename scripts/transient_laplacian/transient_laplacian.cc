#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>

#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

std::string output_dir = "output/case1";

void output_results (const VectorXd &v, const unsigned int counter,
                     const double time);
void write_config (const double start_time, const double end_time,
                   const double dt);

void run ()
{
    using namespace FVMCode;
    UnstructuredMesh       mesh;
    UnstructuredMeshParser parser (
        mesh, "constant/polyMesh/points", "constant/polyMesh/faces",
        "constant/polyMesh/owner", "constant/polyMesh/neighbour",
        "constant/polyMesh/boundary");

    // Setup system
    MatrixXd system_matrix (mesh.n_cells (), mesh.n_cells ());
    VectorXd system_rhs (mesh.n_cells ());
    VectorXd temperature (mesh.n_cells ());

    const double dt                   = 0.05;
    const double output_time_interval = 0.05;
    const double diff_const           = 0.01;
    const double source_strength      = 1.;

    double       time     = 0;
    const double end_time = 1.;

    // Initial conditions
    temperature                   = VectorXd::Constant (mesh.n_cells (), 1);
    unsigned int output_counter   = 0;
    double       next_output_time = 0;
    write_config (time, end_time, dt);
    output_results (temperature, output_counter, time);
    output_counter++;
    next_output_time += output_time_interval;

    while (time < end_time)
    {
        time += dt;

        system_matrix = MatrixXd::Zero (mesh.n_cells (), mesh.n_cells ());
        system_rhs    = VectorXd::Zero (mesh.n_cells ());
        // temporal term
        for (unsigned int i = 0; i < mesh.n_cells (); i++)
        {
            const double volume  = mesh.get_cell (i)->volume ();
            system_matrix (i, i) = volume / dt;
            system_rhs (i)       = system_matrix (i, i) * temperature (i);

            // source
            system_rhs (i) += source_strength * volume;
            // std::cout << "Cell " << i << std::endl;
            // std::cout << "\ta_P = " << system_matrix (i, i) << std::endl;
            // std::cout << "\tb_P = " << system_rhs (i) << std::endl;
        }

        // diffusion term (internal cells)
        for (unsigned int f = 0; f < mesh.n_faces (); f++)
        {
            const auto &face = mesh.get_face (f);
            if (face->is_boundary ())
            {
                // std::cout << "Face " << f << " is a boundary face. Breaking"
                // << std::endl << std::endl;
                break;
            }
            const unsigned int owner_index     = face->neighbour_indices ()[0];
            const unsigned int neighbour_index = face->neighbour_indices ()[1];
            const double a_N = diff_const * face->area () * face->delta ();
            system_matrix (owner_index, owner_index) += a_N;
            system_matrix (neighbour_index, neighbour_index) += a_N;
            system_matrix (owner_index, neighbour_index) = -a_N;
            system_matrix (neighbour_index, owner_index) = -a_N;

            // std::cout << "Face " << f << std::endl;
            // std::cout << "\tOwner Neighbour " << owner_index << " " <<
            // neighbour_index << std::endl; std::cout << "\ta_N " << a_N <<
            // std::endl; std::cout << "\tsystem_matrix(owner,owner) = " <<
            // system_matrix (owner_index, owner_index) << std::endl; std::cout
            // << "\tsystem_matrix(owner,neighbour) = " << system_matrix
            // (owner_index, neighbour_index) << std::endl;
        }

        // diffusion term (boundary conditions)
        for (const auto &boundary : mesh.get_patches ())
        {
            if (boundary.type == empty)
                break;

            for (unsigned int f = boundary.start_face;
                 f < boundary.start_face + boundary.n_faces; f++)
            {
                double boundary_val = 0;
                if (boundary.name == "inlet")
                    boundary_val = 1;
                const auto        &face        = mesh.get_face (f);
                const unsigned int owner_index = face->neighbour_indices ()[0];
                const double a_N = diff_const * face->area () * face->delta ();
                system_matrix (owner_index, owner_index) += a_N;
                system_rhs (owner_index) += a_N * boundary_val;

                // std::cout << "Boundary face " << f << std::endl;
                // std::cout << "\tOwner " << owner_index << std::endl;
                // std::cout << "\tBoundary value " << boundary_val <<
                // std::endl; std::cout << "\tsystem_matrix(owner, owner) " <<
                // system_matrix (owner_index, owner_index) << std::endl;
                // std::cout << "\tsystem_rhs(owner) " <<
                // system_rhs(owner_index) << std::endl;
            }
        }

        // solve system
        temperature = system_matrix.colPivHouseholderQr ().solve (system_rhs);

        // output temperature
        if (time >= next_output_time)
        {
            output_results (temperature, output_counter, time);
            output_counter++;
            next_output_time += output_time_interval;
        }
    }
}

void output_results (const VectorXd &v, const unsigned int counter,
                     const double time)
{
    std::stringstream ss;
    std::string       counter_str = std::to_string (counter);
    unsigned long     num_zeros   = 5;
    ss << output_dir << "/"
       << std::string (num_zeros - std::min (num_zeros, counter_str.length ()),
                       '0')
       << counter_str;
    std::ofstream output (ss.str ());
    assert (output.is_open ());
    output << "t " << time << std::endl;
    for (unsigned int i = 0; i < v.size (); i++)
    {
        output << i << " " << v[i] << std::endl;
    }
}

void write_config (const double start_time, const double end_time,
                   const double dt)
{
    std::stringstream config_file;
    config_file << output_dir << "/config";
    std::ofstream outfile (config_file.str ());
    assert (outfile.is_open ());

    outfile << "cell_no T" << std::endl;
    outfile << "tStart " << start_time << std::endl;
    outfile << "tEnd " << end_time << std::endl;
    outfile << "dt " << dt << std::endl;
}

int main ()
{
    run ();
    return EXIT_SUCCESS;
}