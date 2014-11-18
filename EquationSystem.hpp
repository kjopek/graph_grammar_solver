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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

enum SolverMode {
    OLD,
    LU,
    CHOLESKY
};

class EquationSystem {
private:
  friend class boost::serialization::access;
 
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & mode;
    ar & n;
    
    if (Archive::is_loading::value)
    {
      allocate();
    }
    
    ar & boost::serialization::make_array<double>(matrix[0], n*n);
    ar & boost::serialization::make_array<double>(rhs, n);
  }            
  
  // needed because of implementation of swapRows
  double *origPtr;
  SolverMode mode = OLD;
public:
	// this variables _should_ be public
	// Productions will use them directly

	unsigned long n;
	double ** matrix;
	double *rhs;

	EquationSystem(): rhs(NULL), matrix(NULL) {};
  EquationSystem(unsigned long n, SolverMode mode=OLD);
  
  void allocate();
  
	virtual ~EquationSystem();

	void swapRows(const int i, const int j);
	void swapCols(const int i, const int j);

  int eliminate(const int rows);
	void backwardSubstitute(const int startingRow);

	void checkRow(int row_nr, int* values, int values_cnt);
	void print() const;
};

#endif /* EQUATIONSYSTEM_H_ */
