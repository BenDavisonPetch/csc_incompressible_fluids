#include <FVMCode/unstructured_mesh.h>

namespace FVMCode
{

unsigned int UnstructuredMesh::get_cell_containing_point(const Point<3>& point)
{
    for (unsigned int cell_no = 0; cell_no < n_cells(); cell_no++)
    {
        const auto& cell = get_cell(cell_no);
        if (cell->point_inside(point)) return cell_no;
    }
    Assert(false, "Point lies outside the mesh!");
    return -1;
}

} // namespace FVMCode