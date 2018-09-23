#ifndef MESH_HPP
#define MESH_HPP

#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <map>
#include <string>
#include <vector>
#include "Node.hpp"
#include <cmath>

class Mesh {
    private:
        std::vector<Node *> nodes;
        int dofs = 0;
        Node *root = NULL;

    public:
        Mesh() { }

        void addNode(Node *n);

        static Mesh *loadFromFile(const char *filename);
        bool loadFrontalMatrices(const char *filename);

	Node *getRootNode();
        int getPolynomial();
        void setDofs(int dofs);
        int getDofs();
        
        void setRootNode(Node * root);

        ~Mesh() {
            for (Node *n : nodes) {
                delete (n);
            }
        }

};


#endif // MESH_HPP
