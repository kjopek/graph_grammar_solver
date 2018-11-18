#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <cblas.h>
#include <clapack.h>
}

int
compute_schur(enum CBLAS_ORDER order, double *matrix_a, double *matrix_b,
    double *matrix_c, double *matrix_d, int elim_size, int schur_size, int lda)
{
	int ret;

	/*
	 * matrix_A is elim_size × elim_size
	 * matrix_B is elim_size × schur_size
	 * matrix_C is schur_size × elim_size
	 * matrix_D is schur_size × schur_size
	 */

	ret = clapack_dpotrf(order, CblasUpper, elim_size, matrix_a, lda);
	if (ret != 0)
		return (ret);

	if (schur_size > 0) {
		if (matrix_b == NULL || matrix_c == NULL || matrix_d == NULL) {
			return (-128);
		}
		ret = clapack_dpotrs(order, CblasUpper, elim_size, schur_size, matrix_a,
		    lda, matrix_b, lda);
		if (ret != 0)
			return (ret);

		cblas_dgemm(order, CblasNoTrans, CblasNoTrans, schur_size, elim_size,
		    schur_size, -1.0, matrix_c, lda, matrix_b, lda, 1.0, matrix_d, lda);
	}

	return (0);
}

int
solve(enum CBLAS_ORDER order, double *matrix_a, double *matrix_b,
    double *matrix_c, double *matrix_d, double *rhs, int elim_size,
    int schur_size, int nrhs, int lda)
{
	int ret;

	ret = clapack_dpotrs(order, CblasUpper, elim_size, nrhs, matrix_a, lda,
	    rhs, lda);
	if (ret != 0)
		return (ret);

	if (schur_size > 0) {
		cblas_dgemm(order, CblasNoTrans, CblasNoTrans, elim_size, nrhs, schur_size,
		    -1.0, matrix_c, lda, rhs, lda, 1.0, rhs + nrhs * elim_size, lda);
	}
	return (0);
}


int
main()
{
	/* In column-major order. */
	double matrix[] __attribute__((aligned(128))) = {
	    3.327661745799694e-01, 3.913557075059697e-01, 3.277612458338143e-01, 5.010988765493212e-01,
	    3.913557075059697e-01, 8.861916518303593e-01, 5.002990547443840e-01, 9.292863480674872e-01,
	    3.277612458338143e-01, 5.002990547443840e-01, 5.102524151237025e-01, 6.442488358788692e-01,
	    5.010988765493212e-01, 9.292863480674872e-01, 6.442488358788692e-01, 1.333185355271249e+00
	};
	enum CBLAS_ORDER order = CblasColMajor;


	double rhs[] __attribute__((aligned(128))) = {
	    6.663244457244024e-01,
	    2.291638639293834e-01,
	    4.261006568528997e-01,
	    7.810688841540293e-01
   	};

   	double result[] __attribute__((aligned(128))) = {
	    3.600700244390033e+00,
	    -1.775366081079317e+00,
	    -8.477885310453021e-01,
	    8.796768344542633e-01
	};

	int ret;

	const int elim_size = 2;

	const int lda = 4;
	const int schur_size = lda - elim_size;
	const int nrhs = 1;

	double error = 0.0;
	double *matrix_a, *matrix_b, *matrix_c, *matrix_d;

	matrix_a = matrix;
	matrix_b = matrix_a + (order == CblasColMajor ? elim_size * lda : elim_size);
	matrix_c = matrix_a + (order == CblasColMajor ? elim_size : elim_size * lda);
	matrix_d = matrix_a + elim_size * (lda + 1);

	ret = compute_schur(CblasColMajor, matrix_a, matrix_b, matrix_c, matrix_d,
	    elim_size, schur_size, lda);

	assert(ret == 0);

	compute_schur(CblasColMajor, matrix_d,
	    NULL, NULL, NULL, schur_size, 0, lda);
	assert(ret == 0);

	ret = solve(CblasColMajor, matrix_a, matrix_b, matrix_c, matrix_d, rhs,
	    elim_size, schur_size, nrhs, lda);
	assert(ret == 0);

	ret = solve(CblasColMajor, matrix_d, NULL, NULL, NULL, rhs + nrhs * elim_size,
	    schur_size, 0, nrhs, lda);
	assert(ret == 0);

	cblas_dgemm(order, CblasNoTrans, CblasNoTrans, elim_size, nrhs, schur_size,
	    -1.0, matrix_b, lda, rhs + nrhs * elim_size, lda, 1.0, rhs, lda);

	for (int ii = 0; ii < lda; ii++)
		error += fabs(rhs[ii] - result[ii]);

	printf("%lf\n", error);
	assert(error < 1e-13);
	return (0);
}
