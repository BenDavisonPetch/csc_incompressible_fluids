#ifndef OUTPUT_H
#define OUTPUT_H

#include <filesystem>
#include <vector>

#include <FVMCode/boundary_patch.h>

#include <Eigen/Core>

using Eigen::VectorXd;

namespace FVMCode
{
class Outputter
{
  public:
    using BoundaryConditions
    = std::vector<std::pair<BoundaryPatch, BoundaryFieldEntry> >;
    
    Outputter (const unsigned int timestep_number, const double time,
               const double dt, const double original_dt,
               const unsigned int file_name_precision = 2);
    static std::string name_from_time (const double time,
                                       unsigned int file_name_precision);

    void write_time ();
    void write_scalar_field (const VectorXd &scalar_field, std::string name,
                             const BoundaryConditions
                                 &boundary_conditions);

  private:
    void        init_directory ();
    std::string get_foam_header (std::string location, std::string class_,
                                 std::string object);

    const std::string  dir_name;
    const double       time;
    const unsigned int timestep_number;
    const double       dt;
    const double       original_dt;
};

} // namespace FVMCode

#endif