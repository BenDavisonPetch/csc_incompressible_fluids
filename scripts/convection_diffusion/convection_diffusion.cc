#include "FVMCode/boundary_patch.h"
#include <FVMCode/file_parser.h>
#include <FVMCode/unstructured_mesh.h>
#include <FVMCode/output.h>

#include <Eigen/Core>

using Eigen::VectorXd;

using namespace FVMCode;
using BoundaryConditions = std::vector<std::pair<BoundaryPatch, BoundaryFieldEntry>>;

int main() {
    UnstructuredMesh mesh;
    {
        UnstructuredMeshParser parser(mesh);
    }
    
    // BCs
    BoundaryConditions bc;

    std::vector<BoundaryPatch> patches = mesh.get_patches();
    for (unsigned int p = 0; p < mesh.n_boundary_patches(); p++)
    {
        auto patch = patches[p];
        if (patch.type == empty) {
            BoundaryFieldEntry field("empty", 0);
            bc.push_back(std::make_pair<BoundaryPatch, BoundaryFieldEntry>(patch, BoundaryFieldEntry("empty", 0)))
            continue;
        }
        
        BoundaryFieldEntry field("zeroGradient", 0);
        bc[patch] = field;
    }

    VectorXd temperature(mesh.n_cells());

    for (unsigned int i = 0; i < mesh.n_cells(); i++)
    {
        temperature(i) = i;
    }

    Outputter outputter(0, 0, 0.05, 0.05);
    outputter.write_time();
    outputter.write_scalar_field(temperature, "T", bc);
}