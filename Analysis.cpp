/*
 * Tree analyser.
 *
 * Please note that this code is far far away from being optimal.
 * First of all, such analysis should be done just once per tree
 * and cached together with tree structure. This has yet to be done.
 */

#include "Analysis.hpp"
#include <algorithm>

void Analysis::nodeAnaliser(Node *node, std::set<int> &parent)
{
    auto getAllDOFs = [] (Node *n, std::set<int> &dofs) {
        std::vector<int> &elementDofs = n->getElementDofs();
        dofs.insert(elementDofs.begin(), elementDofs.end());
    };

    std::set<int> common;

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        std::set<int> lDofs;
        std::set<int> rDofs;

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
    std::set<int> parent;

    Analysis::nodeAnaliser(root, parent);
    Analysis::mergeAnaliser(root);
}

void Analysis::mergeAnaliser(Node *node)
{
    if (node->getLeft() != NULL && node->getRight() != NULL) {
        int leftDofsSize, leftDofsElim;
        int rightDofsSize, rightDofsElim;

        std::map<int, int> reverseMap;

        leftDofsSize = node->getLeft()->getDofs().size();
        leftDofsElim = node->getLeft()->getDofsToElim();
        rightDofsSize = node->getRight()->getDofs().size();
        rightDofsElim = node->getRight()->getDofs().size();

        node->leftPlaces.resize(leftDofsSize - leftDofsElim);
        node->rightPlaces.resize(rightDofsSize - rightDofsElim);

        for (int i = 0; i < node->getDofs().size(); ++i)
            reverseMap[node->getDofs()[i]] = i;

        for (int i = leftDofsElim; i < leftDofsSize; ++i) {
            node->leftPlaces[i - leftDofsElim] =
                reverseMap[node->getLeft()->getDofs()[i]];
	}

        for (int i = rightDofsElim; i < rightDofsSize; ++i) {
            node->rightPlaces[i - rightDofsElim] =
                reverseMap[node->getRight()->getDofs()[i]];
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
