#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <Eigen/Core>

using Eigen::VectorXd;

#include <FVMCode/sparsity/sparsity_pattern.h>

namespace FVMCode
{

/**
 * A sparse matrix in Arrow format
 */
class SparseMatrix
{
  public:
    SparseMatrix (const SparsityPattern &sp);

    unsigned int n () const { return sp.n_eqns (); };

    bool symmetric () const;
    /**
     * Determines if the matrix is *strictly* diagonally dominant
     */
    bool diagonally_dominant () const;
    /**
     * Determines if the matrix is symmetric positive definite.
     */
    bool spd () const;

    /**
     * Sets @param dst to the result of the vector matrix multiplication with
     * @param src.
     */
    void vmult (const VectorXd &src, VectorXd &dst) const;
    /**
     * Adds result of matrix * @param src to @param dst.
     */
    void vmult_add (const VectorXd &src, VectorXd &dst) const;

    const double &operator() (const unsigned int i,
                              const unsigned int j) const;
    double       &operator() (const unsigned int i, const unsigned int j);

  private:
    SparsityPattern sp;

    std::vector<double> diagonal;
    std::vector<double> upper_triangular;
    std::vector<double> lower_triangular;
};

} // namespace FVMCode

#endif