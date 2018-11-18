#ifndef SOLVER_H
#define SOLVER_H

#include <string>
#include <vector>

extern "C" {
#include <cblas.h>
#include <clapack.h>
};

#include "Node.h"
#include "log.h"

namespace solver {

enum FactorizationStatus {
	kOK = 0,
	kNoMatrixLoaded,
	kNoRightHandSideLoaded,
	kLUError,
	kCholeskyError,
};

enum LoadStatus {
	kNoMemoryError = 1,
};

enum SolveStatus {
	kRuntimeError = 1,
	kBSError,
};

enum TreeFileType {
	kTreePlain,
	kTreeCached
};

enum SaveStatus {
	kNoMemoryAvailable = 1
};

enum FactorizationMethod {
	kLU = 0,
	kCholesky
};

class Solver {
    private:
        std::vector<Node> nodes;
	std::vector<int> supernodes;

	int systemSize = -1;	// Global system size.
	int rhsSize = -1;	// Num of RHS (RHS is matrix systemSize × rhsSize).

	enum TreeFileType treeType;
	enum CBLAS_ORDER dataOrder;

	/* Analysis part. */
	void analyseTree();
        void mergeAnaliser(Node &node);
        void nodeAnaliser(Node &n, std::set<int> &parent);

	/* Elimination / solve part. */
	enum FactorizationStatus factorizeTree(Node &root);
	void mergeNode(Node &node);
	void loadNodes(std::string treePath);
	void loadCachedNodes(std::string treePath);
	Node &getNode(int id);
    public:
        Solver(std::string treePath, enum TreeFileType treeType=kTreePlain,
	    enum CBLAS_ORDER dataOrder=CblasColMajor);
	~Solver();

	enum LoadStatus loadFrontMatrix(const double *matrix, int rows, int cols,
	    int lda);
	enum LoadStatus loadRightHandSide(const double *rhs, int systemSize,
	    int rhsSize);

	enum FactorizationStatus factorize(enum FactorizationMethod method=kLU);
	enum SolveStatus getSolution(double *solution);
	enum SaveStatus saveCachedTree(const std::string &target);
};

}
#endif /* SOLVER_H */
