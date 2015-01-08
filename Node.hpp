#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "Element.hpp"
#include "EquationSystem.hpp"

#include <set>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class Mesh;

class Node {
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
          ar & node;
          ar & left;
          ar & right;
          ar & parent;
          ar & mergedElements;
          ar & production;
          ar & dofs;
          ar & dofsToElim;
          ar & n_left;
          ar & n_right;
          ar & l;
          ar & r;  
          ar & leftPlaces;  
          ar & rightPlaces;  
        }
      
        int node = -1;
        Node *left = NULL;
        Node *right = NULL;
        Node *parent = NULL;
        std::vector<Element *> mergedElements;
        std::string production;
        std::vector<uint64_t> dofs;

        uint64_t dofsToElim;

    public:
        EquationSystem *system; // TODO: temporary, add friend class for serialization or sth.
        
        int n_left = -1;
        int n_right = -1;
        
        int l=0;
        int r=0;

        std::vector<uint64_t> leftPlaces;;
        std::vector<uint64_t> rightPlaces;
        
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

        void addElement (Element *e);
        std::vector<Element *> &getElements();
        void clearElements();

        void addDof(uint64_t dof);
        std::vector<uint64_t> &getDofs();
        void clearDofs();

        int getId() const;

        void setDofsToElim(uint64_t dofs);
        uint64_t getDofsToElim() const;

        void allocateSystem(SolverMode mode);
        void deallocateSystem();

        void setProduction(std::string &prodname);
        std::string &getProduction();

        void (*mergeProduction)(double **matrixA, double *rhsA,
                                double **matrixB, double *rhsB,
                                double **matrixOut, double *rhsOut);

        void (*preprocessProduction)(double **matrixIn, double *rhsIn,
                                     double **matrixOut, double *rhsOut);
        void fillin() const;
        void merge() const;
        void eliminate() const;
        void bs() const;
        
        unsigned long getSizeInMemory(bool recursive = true);
        unsigned long getFLOPs(bool recursive = true);
        unsigned long getMemoryRearrangements();

        static bool isNeighbour (Node *node, Node *parent);
        static bool isNeighbour (Element *element1, Element *element2);
        int getNumberOfNeighbours(std::vector<Element *> & allElements);
        
        void rebuildElements();
        
        /*DEBUG*/
        
        int treeSize();
        int isBalanced(bool * bal);
        bool isLooped(std::set<Node*> * n);
        
        /*END OF DEBUG*/
};

#endif // NODE_HPP
