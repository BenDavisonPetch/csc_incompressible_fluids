#ifndef SPARSITY_PATTERN_H
#define SPARSITY_PATTERN_H

#include <map>
#include <fstream>

#include <FVMCode/unstructured_mesh.h>

namespace FVMCode
{

/**
 * Provides addressing for SparseMatrix
 */
class SparsityPattern
{
  public:
    SparsityPattern (UnstructuredMesh &mesh);

    /**
     * Number of rows/columns/equations.
     */
    unsigned int n_eqns () const { return n; }
    /**
     * Number of potentially non-zero off-diagonal entries.
     */
    unsigned int n_off_diagonal_entries () const;
    /**
     * Maximum difference between row index and column index for the
     * potentially non-zero off-diagonal entries.
     */
    unsigned int matrix_band () const;
    /**
     * Writes sparsity pattern to file in a format gnuplot can understand.
     * Specifically, writes a pair i j of row and column indices per line, and
     * prints the negative of the column index such that (0,0) is in the top
     * left corner rather than bottom left. Can be visualised by using gnuplot:
     * \code{.gnuplot}
     * set style data points
     * plot "file_name_here"
     * \endcode
     */
    void print_gnuplot (std::ofstream &out) const;

    /**
     * Requires i < j.
     */
    unsigned int arrow_index_from_ij (const unsigned int i,
                                      const unsigned int j) const;
    /**
     * Always gives i < j (i.e. in upper triangle)
     */
    std::pair<unsigned int, unsigned int>
    ij_from_arrow_index (const unsigned int arrow_index) const;

  private:
    unsigned int n;
    // Only stores i,j pairs where i < j (i.e. in the upper triangle)
    std::vector<std::pair<unsigned int, unsigned int> > ij_indices;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int>
        reverse_lookup;
};

} // namespace FVMCode

#endif