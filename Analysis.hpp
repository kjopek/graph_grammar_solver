#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP
#include <set>
#include <map>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include "Mesh.hpp"

class Analysis {
    private:

        static void mergeAnaliser(Node *node);
        
    public:
        static void enumerateDOF(Mesh *mesh);
        static void doAnalise(Mesh *mesh);
        static void nodeAnaliser(Node *n, std::set<int> &parent);

        // for debug use
        static void debugNode(Node *n);
        static void printTree(Node *n);
};

#endif // ANALYSIS_HPP
