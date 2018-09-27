
#include <string.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

extern "C" {
#include <cblas.h>
#include <clapack.h>
}

#include "log.h"
#include "EquationSystem.h"

EquationSystem::EquationSystem(unsigned long n, SolverMode mode) {
    this->n    = n;
    this->mode = mode;
  
    allocate();
}
  
void EquationSystem::allocate()
{
    unsigned long i;

    // we are working on continuous area of memory
    // Assuming cache line is 64 bytes we want to separate allocations
    // in order to eliminate false sharing.
    posix_memalign((void **)&matrix, 128, n * (n + 1) * sizeof(double));
    // Perform memset over allocated area to shake page faults.
    memset((void *)matrix, 0, n * (n + 1) * sizeof(double));

    if (matrix == NULL)
        throw std::string("Cannot allocate memory!");

    rhs = matrix + n * n;
}

EquationSystem::~EquationSystem()
{
    free(matrix);
    free(ipiv);
}

int EquationSystem::eliminate(const int rows)
{

    int error = 0, ret = 0;

    const int m = rows;
    const int k = n - rows;

    if (mode == LU) {
        ret = posix_memalign((void **)&ipiv, 128, m * sizeof(int));
	LOG_ASSERT(ret == 0, "IPIV allocation error.");

        error = clapack_dgetrf(CblasColMajor, m, m, matrix, n, ipiv);
        if (error != 0) {
            printf("DGETRF error: %d\n", error);
            return (error);
        }

        error = clapack_dgetrs(CblasColMajor, CblasNoTrans, m, k, matrix,
            n, ipiv, matrix + m * n, n);
        if (error != 0) {
            printf("DGETRS error: %d\n", error);
            return (error);
        }
    } else if (mode == CHOLESKY) {
        error = clapack_dpotrf(CblasColMajor, CblasUpper, m, matrix, n);
        if (error != 0) {
            printf("DPOTRF error: %d\n", error);
            return (error);
        }

        clapack_dpotrs(CblasColMajor, CblasUpper, m, k, matrix, n,
            matrix + m * n, n);
    } else {
        printf("Unknown factorization method: %d.\n", mode);
        return (-1);
    }
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, k, k, m, -1.0,
        matrix + m, n, matrix + m * n, n, 1.0, matrix + (n + 1) * m, n);

    return (0);
}

int EquationSystem::solve(const int rows)
{
    int error;

    const int m = rows;
    const int k = n - rows;

    if (mode == LU) {
	LOG_ASSERT(ipiv != NULL,
            "IPIV is NULL. Factorization was not performed?");
        error = clapack_dgetrs(CblasColMajor, CblasNoTrans, m, 1, matrix, n,
            ipiv, rhs, n);
	if (error != 0) {
		printf("DGETRS error: %d\n", error);
		return (error);
	}
    } else {
        clapack_dpotrs(CblasColMajor, CblasUpper, m, 1, matrix, n, rhs, n);
    }
}

void EquationSystem::print() const
{
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::printf("% .6f ", *(matrix + i * n + j));
        }
        std::printf (" | % .6f\n", rhs[i]);
    }
}

