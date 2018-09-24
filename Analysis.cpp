/*
 * Tree analyser.
 *
 * Please note that this code is far far away from being optimal.
 * First of all, such analysis should be done just once per tree
 * and cached together with tree structure. This has yet to be done.
 */

#include "Analysis.hpp"
#include <algorithm>

using namespace std;

void Analysis::nodeAnaliser(Node *node, set<int> &parent)
{
    auto getAllDOFs = [] (Node *n, set<int> &dofs) {
        vector<int> &elementDofs = n->getElementDofs();
        dofs.insert(elementDofs.begin(), elementDofs.end());
    };

    set<int> common;

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        set<int> lDofs;
	set<int> rDofs;
        getAllDOFs(node->getLeft(), lDofs);
        getAllDOFs(node->getRight(), rDofs);

        std::set_intersection(lDofs.begin(), lDofs.end(),
            rDofs.begin(), rDofs.end(),
            std::inserter(common, common.begin()));

        for (auto p = parent.cbegin(); p != parent.cend(); ++p) {
            if (lDofs.count(*p) || rDofs.count(*p))
                common.insert(*p);
        }


        Analysis::nodeAnaliser(node->getLeft(), common);
        Analysis::nodeAnaliser(node->getRight(), common);

    } else {
        getAllDOFs(node, common);
    }

    int i = 0;

    for (int dof: common) {
        if (!parent.count(dof)) {
            node->addDof(dof);
            ++i;
        }
    }

    node->setDofsToElim(i);

    for (int dof: common) {
        if (parent.count(dof)) {
            node->addDof(dof);
        }
    }
}

void Analysis::doAnalise(Mesh *mesh)
{
    Node *root = mesh->getRootNode();
    std::set<int> parent;;

    Analysis::nodeAnaliser(root, parent);
    Analysis::mergeAnaliser(root);
}

void Analysis::mergeAnaliser(Node *node)
{
    if (node->getLeft() != NULL && node->getRight() != NULL) {
        node->leftPlaces.resize(node->getLeft()->getDofs().size() - node->getLeft()->getDofsToElim());
        node->rightPlaces.resize(node->getRight()->getDofs().size() - node->getRight()->getDofsToElim());

        map<int, int> reverseMap;

        for (int i = 0; i < node->getDofs().size(); ++i) {
            reverseMap[node->getDofs()[i]] = i;
        }

        for (int i = node->getLeft()->getDofsToElim(); i < node->getLeft()->getDofs().size(); ++i) {
            node->leftPlaces[i-node->getLeft()->getDofsToElim()] = reverseMap[node->getLeft()->getDofs()[i]];
        }

        for (int i = node->getRight()->getDofsToElim(); i < node->getRight()->getDofs().size(); ++i) {
            node->rightPlaces[i-node->getRight()->getDofsToElim()] = reverseMap[node->getRight()->getDofs()[i]];
        }

        Analysis::mergeAnaliser(node->getLeft());
        Analysis::mergeAnaliser(node->getRight());
    }
}

void Analysis::debugNode(Node *n)
{
    printf("Node: %d\n", n->getId());
    printf("  dofs: ");
    for (int dof: n->getDofs()) {
        printf("%d ", dof);
    }
    printf("\n");
}

void Analysis::printTree(Node *n)
{
    printf("Node id: %d ", n->getId());
    printf("[");
    for (int dof: n->getDofs()) {
        printf("%d, ", dof);
    }
    printf("]");
    printf(" elim: %d\n", n->getDofsToElim());

    if(n->getLeft() != NULL && n->getRight() != NULL) {
        printTree(n->getLeft());
        printTree(n->getRight());
    }

}
