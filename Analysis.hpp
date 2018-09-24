#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <vector>

#include "Mesh.hpp"

class Analysis {
    private:
        static void mergeAnaliser(Node *node);
        static void nodeAnaliser(Node *n, std::set<int> &parent);

    public:
        static void doAnalise(Mesh *mesh);

        // for debug use
        static void debugNode(Node *n);
        static void printTree(Node *n);
};

#endif // ANALYSIS_HPP
