#include <FVMCode/sparsity/sparse_matrix.h>

inline bool fclose (double a, double b)
{
    return std::fabs (a - b) <= (std::fabs (a) + std::fabs (b)) * 1e-12;
}

namespace FVMCode
{

SparseMatrix::SparseMatrix (const SparsityPattern &sp)
    : sp (sp)
    , diagonal (sp.n_eqns ())
    , upper_triangular (sp.n_off_diagonal_entries () / 2)
    , lower_triangular (sp.n_off_diagonal_entries () / 2)
{
}

bool SparseMatrix::symmetric () const
{
    for (unsigned int i = 0; i < upper_triangular.size (); i++)
    {
        if (!fclose (upper_triangular[i], lower_triangular[i]))
            return false;
    }
    return true;
}

bool SparseMatrix::diagonally_dominant () const
{
    std::vector<double> dominance_sums (n ());
    for (unsigned int row = 0; row < n (); row++)
        dominance_sums[row] = std::fabs (diagonal[row]);

    for (unsigned int index = 0; index < upper_triangular.size (); index++)
    {
        auto [i, j] = sp.ij_from_arrow_index (index);
        dominance_sums[i] -= std::fabs (upper_triangular[index]);
        dominance_sums[j] -= std::fabs (lower_triangular[index]);
    }

    // Now we check if any of the dominance_sums elements are -ve. If so, the
    // matrix is not diagonally dominant.
    for (const auto val : dominance_sums)
        if (val <= 0.)
            return false;
    return true;
}

bool SparseMatrix::spd () const
{
    // We use the fact that a symmetric matrix is positive definite iff it is
    // strictly diagonally dominant
    return symmetric () && diagonally_dominant ();
}

void SparseMatrix::vmult (const VectorXd &src, VectorXd &dst) const
{
    Assert (src.size () == n (),
            "Vectors are of different size to sparse matrix");
    dst = VectorXd::Zero (src.size ());
    vmult_add (src, dst);
}

void SparseMatrix::vmult_add (const VectorXd &src, VectorXd &dst) const
{
    Assert (src.size () == dst.size (), "Vectors are of inconsistent size");
    Assert (src.size () == n (),
            "Vectors are of different size to sparse matrix");

    for (unsigned int row = 0; row < n (); row++)
    {
        dst (row) += diagonal[row] * src (row);
    }
    for (unsigned int index = 0; index < upper_triangular.size (); index++)
    {
        auto [i, j] = sp.ij_from_arrow_index (index);
        dst (i) += upper_triangular[index] * src (j);
        dst (j) += lower_triangular[index] * src (i);
    }
}

const double &SparseMatrix::operator() (const unsigned int i,
                                        const unsigned int j) const
{
    if (i == j)
        return diagonal[i];
    if (i < j)
        return upper_triangular[sp.arrow_index_from_ij (i, j)];
    else
        return lower_triangular[sp.arrow_index_from_ij (j, i)];
}

double &SparseMatrix::operator() (const unsigned int i, const unsigned int j)
{
    if (i == j)
        return diagonal[i];
    if (i < j)
        return upper_triangular[sp.arrow_index_from_ij (i, j)];
    else
        return lower_triangular[sp.arrow_index_from_ij (j, i)];
}

} // namespace FVMCode