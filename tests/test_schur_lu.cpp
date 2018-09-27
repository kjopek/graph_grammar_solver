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
    double *matrix_c, double *matrix_d, int elim_size, int schur_size, int lda,
    int *ipiv)
{
	int ret;

	/*
	 * matrix_A is elim_size × elim_size
	 * matrix_B is elim_size × schur_size
	 * matrix_C is schur_size × elim_size
	 * matrix_D is schur_size × schur_size
	 */

	ret = clapack_dgetrf(order, elim_size, elim_size, matrix_a, lda, ipiv);
	if (ret != 0)
		return (ret);

	if (schur_size > 0) {
		if (matrix_b == NULL || matrix_c == NULL || matrix_d == NULL) {
			return (-128);
		}
		ret = clapack_dgetrs(order, CblasNoTrans, elim_size, schur_size, matrix_a,
		    lda, ipiv, matrix_b, lda);
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
    int schur_size, int nrhs, int lda, int *ipiv)
{
	int ret;

	ret = clapack_dgetrs(order, CblasNoTrans, elim_size, nrhs, matrix_a, lda,
	    ipiv, rhs, lda);
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
	   6.283457663709116e-01, 6.092352604166500e-01, 4.938755795117774e-01, 6.958468734012109e-01,
	   8.717125779114329e-01, 4.950533225003989e-01, 8.765632628924573e-01, 7.297943352914107e-01,
	   3.540576044149239e-02, 8.137949294264613e-01, 1.185234479905606e-01, 8.002608477042044e-01,
	   8.486071913980264e-01, 5.543351460075193e-01, 9.550311289789993e-01, 3.452079015446026e-01

	};
	enum CBLAS_ORDER order = CblasColMajor;


	double rhs[] __attribute__((aligned(128))) = {
	   5.853882509672809e-01,
	   8.962408851010853e-01,
	   1.303247907060850e-01,
	   1.451267761241856e-01
   	};
   
   	double result[] __attribute__((aligned(128))) = {
	   3.931848620273344e+00,
	  -3.468772804855613e+00,
	  -6.649069175425454e-01,
	   1.369470170811092e+00
	};

	int ret;

	const int elim_size = 2;

	const int lda = 4;
	const int schur_size = lda - elim_size;
	const int nrhs = 1;
	int ipiv1[elim_size];
	int ipiv2[elim_size];
	double error = 0.0;
	double *matrix_a, *matrix_b, *matrix_c, *matrix_d;

	matrix_a = matrix;
	matrix_b = matrix_a + (order == CblasColMajor ? elim_size * lda : elim_size);
	matrix_c = matrix_a + (order == CblasColMajor ? elim_size : elim_size * lda);
	matrix_d = matrix_a + elim_size * (lda + 1);

	ret = compute_schur(CblasColMajor, matrix_a, matrix_b, matrix_c, matrix_d,
	    elim_size, schur_size, lda, ipiv1);

	assert(ret == 0);

	compute_schur(CblasColMajor, matrix_d,
	    NULL, NULL, NULL, schur_size, 0, lda, ipiv2);
	assert(ret == 0);
	
	ret = solve(CblasColMajor, matrix_a, matrix_b, matrix_c, matrix_d, rhs,
	    elim_size, schur_size, nrhs, lda, ipiv1);
	assert(ret == 0);

	ret = solve(CblasColMajor, matrix_d, NULL, NULL, NULL, rhs + nrhs * elim_size,
	    schur_size, 0, nrhs, lda, ipiv2);
	assert(ret == 0);

	cblas_dgemm(order, CblasNoTrans, CblasNoTrans, elim_size, nrhs, schur_size,
	    -1.0, matrix_b, lda, rhs + nrhs * elim_size, lda, 1.0, rhs, lda);

	for (int ii = 0; ii < lda; ii++)
		error += fabs(rhs[ii] - result[ii]);

	printf("%lf\n", error);
	assert(error < 1e-13);
	return (0);
}
