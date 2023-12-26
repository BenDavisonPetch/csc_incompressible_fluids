#include <FVMCode/sparsity/sparsity_pattern.h>
#include <FVMCode/unstructured_mesh.h>

namespace FVMCode
{

SparsityPattern::SparsityPattern (UnstructuredMesh &mesh)
    : n (mesh.n_cells ())
{
    Assert (mesh.n_cells () > 0, "Empty mesh!");
    // Loop over internal cells to get off-diagonal entries
    for (unsigned int f = 0; f < mesh.get_patches ()[0].start_face; f++)
    {
        const auto &face = mesh.get_face (f);
        Assert (!face->is_boundary (),
                "Face should not be at boundary! Check face numbering.");

        const unsigned int i = face->neighbour_indices ()[0];
        const unsigned int j = face->neighbour_indices ()[1];
        if (i < j)
        {
            auto ij = std::make_pair(i,j);
            ij_indices.push_back(ij);
            reverse_lookup[ij] = f;
        } else if (j < i)
        {
            auto ji = std::make_pair(j,i);
            ij_indices.push_back(ji);
            reverse_lookup[ji] = f;
        }
        else {
            Assert(false, "Face has owner and neighbour indices the same!");
        }
    }
}

unsigned int SparsityPattern::n_off_diagonal_entries () const
{
    return ij_indices.size () * 2;
}

unsigned int SparsityPattern::matrix_band () const
{
    unsigned int max_diff = 0;
    for (unsigned int i = 0; i < ij_indices.size (); i++)
    {
        unsigned int abs_diff
            = (ij_indices[i].first > ij_indices[i].second)
                  ? ij_indices[i].first - ij_indices[i].second
                  : ij_indices[i].second - ij_indices[i].first;
        max_diff = std::max (max_diff, abs_diff);
    }
    return max_diff;
}

void SparsityPattern::print_gnuplot (std::ofstream &out) const
{
    for (unsigned int i = 0; i < n; i++) out << i << " " << -i << std::endl;
    for (unsigned int index = 0; index < n_off_diagonal_entries (); index++)
    {
        auto [i, j] = ij_from_arrow_index (index);
        out << i << " " << -j << std::endl;
        out << j << " " << -i << std::endl;
    }
}

unsigned int SparsityPattern::arrow_index_from_ij (const unsigned int i,
                                                   const unsigned int j) const
{
    Assert (i < j, "Queried indexes must be in upper triangle!");
    try
    {
        return reverse_lookup.at (std::make_pair (i, j));
    }
    catch (std::out_of_range &e)
    {
        std::cerr << "Index (" << i << ", " << j
                  << ") is not in the sparsity pattern!" << std::endl;
        throw e;
    }
}

std::pair<unsigned int, unsigned int>
SparsityPattern::ij_from_arrow_index (const unsigned int arrow_index) const
{
    return ij_indices[arrow_index];
}

} // namespace FVMCode