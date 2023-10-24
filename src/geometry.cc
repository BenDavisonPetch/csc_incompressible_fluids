#include <FVMCode/geometry.h>

namespace FVMCode {
namespace Geometry{

double tetrahedron_volume(const std::array<Point<3>, 4> &vertices)
{
    Point<3> edge1 = vertices[1] - vertices[0];
    Point<3> edge2 = vertices[2] - vertices[0];
    Point<3> edge3 = vertices[3] - vertices[0];
    return std::fabs(edge1.dot(cross_p(edge2, edge3)));
}

} // namespace Geometry
} // namespace FVMCode