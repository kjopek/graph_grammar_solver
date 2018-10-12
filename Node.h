#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "EquationSystem.h"

#include <set>

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

        int l = 0;
        int r = 0;

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

        void merge() const;
        void eliminate() const;
        void bs() const;

        unsigned long getSizeInMemory(bool recursive = true);
        unsigned long getFLOPs(bool recursive = true);
        unsigned long getMemoryRearrangements();

	void loadMatrix(double *data);
        /* DEBUG */
        int treeSize();
        /* END OF DEBUG */
};

#endif // NODE_H
