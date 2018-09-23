#include "Analysis.hpp"
#include <algorithm>

using namespace std;

void Analysis::nodeAnaliser(Node *node, set<int> *parent)
{
    auto getAllDOFs = [] (Node *n) {
        set<int> *dofs = new set<int>();
        for (Element *e : n->getElements()) {
            for (int dof : e->dofs)
                dofs->insert(dof);
        }
        return dofs;
    };

    set<int> *common;

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        set<int> *lDofs = getAllDOFs(node->getLeft());
        set<int> *rDofs = getAllDOFs(node->getRight());

        common = new set<int>;
        std::set_intersection(lDofs->begin(), lDofs->end(),
            rDofs->begin(), rDofs->end(),
            std::inserter(*common, common->begin()));



        for (auto p = parent->cbegin(); p!=parent->cend(); ++p) {
            if (lDofs->count(*p) || rDofs->count(*p))
                common->insert(*p);
        }

        delete (lDofs);
        delete (rDofs);

        Analysis::nodeAnaliser(node->getLeft(), common);
        Analysis::nodeAnaliser(node->getRight(), common);

    } else {
        common = getAllDOFs(node);
    }

    int i = 0;

    for (int dof: *common) {
        if (!parent->count(dof)) {
            node->addDof(dof);
            ++i;
        }
    }

    node->setDofsToElim(i);

    for (int dof: *common) {
        if (parent->count(dof)) {
            node->addDof(dof);
        }
    }

    delete common;
}

void Analysis::doAnalise(Mesh *mesh)
{
    Node *root = mesh->getRootNode();
    std::set<int> *parent = new set<int>();

    Analysis::nodeAnaliser(root, parent);
    Analysis::mergeAnaliser(root);

    delete parent;
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
    for (uint64_t dof : n->getDofs()) {
        printf("%llu ", dof);
    }
    printf("\n");
}

void Analysis::printTree(Node *n)
{
    printf("Node id: %d ", n->getId());
    printf("[");
    for (uint64_t dof : n->getDofs()) {
        printf("%llu, ", dof);
    }
    printf("]");
    printf(" elim: %llu\n", n->getDofsToElim());

    if(n->getLeft() != NULL && n->getRight() != NULL) {
        printTree(n->getLeft());
        printTree(n->getRight());
    }

}

void Analysis::printElement(Element *e)
{
    printf("E[%llu,%llu] at %llu x %llu -> %llu x %llu = [", e->k, e->l, e->x1, e->y1, e->x2, e->y2);
    for (uint64_t dof : e->dofs) {
        printf("%llu, ", dof);
    }
    printf("]\n");
}

