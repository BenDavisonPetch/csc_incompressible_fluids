#include "FVMCode/boundary_patch.h"
#include <FVMCode/exceptions.h>
#include <FVMCode/output.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

using Eigen::VectorXd;

namespace FVMCode
{

Outputter::Outputter (const unsigned int timestep_number, const double time,
                      const double dt, const double original_dt,
                      const unsigned int file_name_precision)
    : dir_name (name_from_time (time, file_name_precision))
    , time (time)
    , timestep_number (timestep_number)
    , dt (dt)
    , original_dt (original_dt)
{
    init_directory ();
}

std::string Outputter::name_from_time (const double time,
                                       unsigned int file_name_precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision (file_name_precision) << time;
    return ss.str ();
}

void Outputter::init_directory ()
{
    std::filesystem::create_directory (dir_name);
}

std::string Outputter::get_foam_header (std::string location,
                                        std::string class_, std::string object)
{
    std::stringstream header;
    header << "FoamFile" << std::endl
           << "{" << std::endl
           << "\tversion\t2.0;" << std::endl
           << "\tformat\tascii;" << std::endl
           << "\tarch\t\"LSB;label=32;scalar=64\";" << std::endl
           << "\tclass\t" << class_ << ";" << std::endl
           << "\tlocation\t\"" << location << "\";" << std::endl
           << "\tobject\t" << object << ";" << std::endl
           << "}" << std::endl;
    return header.str ();
}

void Outputter::write_time ()
{
    auto uniform_dir
        = std::filesystem::path (dir_name) / std::filesystem::path ("uniform");
    std::filesystem::create_directory (uniform_dir);

    std::ofstream outfile (uniform_dir / "time");
    outfile << get_foam_header (dir_name + "/uniform", "dictionary", "time");

    outfile << "value\t" << time << ";" << std::endl << std::endl;
    outfile << "name\t\"" << dir_name << "\";" << std::endl << std::endl;
    outfile << "index\t" << timestep_number << ";" << std::endl << std::endl;
    outfile << "deltaT\t" << dt << ";" << std::endl << std::endl;
    outfile << "deltaT0\t" << original_dt << ";" << std::endl << std::endl;

    outfile.close ();
}

void Outputter::write_scalar_field (
    const VectorXd &scalar_field, std::string name,
    const std::map<BoundaryPatch, BoundaryFieldEntry> &boundary_conditions)
{
    Assert (name, "Field must have a name!");

    auto time_dir = std::filesystem::path (dir_name);

    std::ofstream outfile (time_dir / name);
    outfile << get_foam_header (dir_name, "volScalarField", name);

    outfile << "dimensions\t[0 0 0 0 0 0 0];" << std::endl << std::endl;

    // Internal field
    outfile << "internalField\tnonuniform List<scalar>" << std::endl;
    outfile << scalar_field.size () << std::endl;
    outfile << "(" << std::endl;
    outfile << scalar_field << std::endl;
    outfile << ");" << std::endl;

    // Boundary field
    outfile << "boundaryField" << std::endl << "{" << std::endl;
    for (auto& [patch, field] : boundary_conditions)
    {
        outfile << "\t" << patch.name << std::endl << "{" << std::endl;
        outfile << "\t\ttype\t" << field.type << ";" << std::endl;
        if (field.type == "fixedValue")
            outfile << "\t\tvalue\tuniform" << field.value << ";" << std::endl;
        outfile << "}" << std::endl;
    }
    outfile << "}" << std::endl;
}

} // namespace FVMCode