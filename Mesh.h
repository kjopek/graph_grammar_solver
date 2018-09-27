#ifndef MESH_H
#define MESH_H

#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <map>
#include <string>
#include <vector>
#include "Node.h"
#include <cmath>

class Mesh {
    private:
        std::vector<Node *> nodes;
        int dofs = 0;
        Node *root = NULL;
        void addNode(Node *n);

    public:
        Mesh() { }
        static Mesh *loadTreeFromFile(const char *filename);

        Node *getRootNode();
        void setDofs(int dofs);
        int getDofs();
        void loadLeafMatrix(int nodeId, double *matrix);
        
        void setRootNode(Node * root);

        ~Mesh() {
            for (Node *n : nodes) {
                delete (n);
            }
        }

};


#endif // MESH_H
