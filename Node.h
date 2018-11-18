#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

#include "EquationSystem.h"


class Node {
    private:
        int id = -1;
	int leftId = -1;
	int rightId = -1;
	int parentId = -1;

	int elimSize;

        std::vector<int> dofs;
	std::vector<int> elementDofs;

    public:
        EquationSystem *system;

	std::vector<int> leftPlaces;;
        std::vector<int> rightPlaces;

        Node(): id(-1), system(NULL) {}

        Node(int id) : id(id), system(NULL) {}
        ~Node() {
            delete system;
        }

	int getId() const;

	int getLeftId() const;
	int getRightId() const;
	int getParentId() const;

	void setLeftId(int leftId);
	void setRightId(int rightId);
	void setParentId(int parentId);

        void clearElements();

        void addDof(int dof);
        std::vector<int> &getDofs();

	void setElementDofs(std::vector<int> &dofs);
	std::vector<int> &getElementDofs();

        void clearDofs();

        void setElimSize(int dofs);
        int getElimSize() const;

        void allocateSystem(SolverMode mode);
        void deallocateSystem();

        void merge(Node &left, Node &right) const;
        int eliminate() const;
        void bs() const;

	void loadMatrix(double *data);
};

#endif // NODE_H
