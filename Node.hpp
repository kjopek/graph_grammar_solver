#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "EquationSystem.hpp"

#include <set>

class Mesh;

class Node {
    private:
        int node = -1;
        Node *left = NULL;
        Node *right = NULL;
        Node *parent = NULL;
        std::vector<int> dofs;
	std::vector<int> elementDofs;

        int dofsToElim;

    public:
        EquationSystem *system;
        
        int n_left = -1;
        int n_right = -1;
        
        int l = 0;
        int r = 0;

        std::vector<int> leftPlaces;;
        std::vector<int> rightPlaces;
        
        Node(): node(0), system(NULL) {}
        
        Node(int num) : node(num), system(NULL) {}
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

        void fillin() const;
        void merge() const;
        void eliminate() const;
        void bs() const;
        
        unsigned long getSizeInMemory(bool recursive = true);
        unsigned long getFLOPs(bool recursive = true);
        unsigned long getMemoryRearrangements();
        
        /* DEBUG */
        int treeSize();
        /* END OF DEBUG */
};

#endif // NODE_HPP
