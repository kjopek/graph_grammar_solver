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
        Node *left = NULL;
        Node *right = NULL;
        Node *parent = NULL;
        std::vector<int> dofs;
	std::vector<int> elementDofs;

        int dofsToElim;

    public:
        EquationSystem *system;
        
        int leftId = -1;
        int rightId = -1;
        
        int l = 0;
        int r = 0;

        std::vector<int> leftPlaces;;
        std::vector<int> rightPlaces;
        
        Node(): id(-1), system(NULL) {}
        
        Node(int id) : id(id), system(NULL) {}
        ~Node() {
            delete system;
        }

        void setLeft(Node *left);
        void setRight(Node *right);
        void setParent(Node *parent);

        Node *getLeft() const;
        Node *getRight() const;
        Node *getParent() const;

        void clearElements();

        void addDof(int dof);
        std::vector<int> &getDofs();

	void setElementDofs(std::vector<int> &dofs);
	std::vector<int> &getElementDofs();

        void clearDofs();

        int getId() const;

        void setDofsToElim(int dofs);
        int getDofsToElim() const;

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
