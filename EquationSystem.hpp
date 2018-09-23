/*
 * EquationSystem.h
 *
 * Implementation of dense matrix storage with column-major order
 *
 *  Created on: 05-08-2013
 *      Author: kj
 */

#ifndef EQUATIONSYSTEM_H_
#define EQUATIONSYSTEM_H_

#include <cstdio>
#include <cstdlib>

enum SolverMode {
    LU,
    CHOLESKY
};

class EquationSystem {
private:
    SolverMode mode = LU;
public:
    // this variables _should_ be public
    // Productions will use them directly

    unsigned long n;
    double *matrix;
    double *rhs;

    EquationSystem(): rhs(NULL), matrix(NULL) {};
    EquationSystem(unsigned long n, SolverMode mode=LU);

    void allocate();

    virtual ~EquationSystem();

    int eliminate(const int rows);

    // DEBUG
    void print() const;
};

#endif /* EQUATIONSYSTEM_H_ */
