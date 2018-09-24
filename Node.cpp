#include <algorithm>
#include <set>
#include <string.h>

#include "Analysis.hpp"
#include "Node.hpp"
#include "log.h"

void Node::setLeft(Node *left)
{
    left = left;
}

void Node::setRight(Node *right)
{
    right = right;
}

void Node::setParent(Node *parent)
{
    parent = parent;
}

Node *Node::getLeft() const
{
    return (left);
}

Node *Node::getRight() const
{
    return (right);
}

Node *Node::getParent() const
{
    return (parent);
}

int Node::getId() const
{
    return (node);
}

void Node::addDof(int dof)
{
    dofs.push_back(dof);
}

std::vector<int> &Node::getDofs()
{
    return (dofs);
}

void Node::clearDofs(){
    dofs.clear();
}

void Node::setElementDofs(std::vector<int> &dofs)
{
    elementDofs = dofs;
}

std::vector<int> &Node::getElementDofs()
{
    return (elementDofs);
}

void Node::setDofsToElim(int dofs)
{
    dofsToElim = dofs;
}

int Node::getDofsToElim() const
{
    return (dofsToElim);
}

void Node::allocateSystem(SolverMode mode)
{
    system = new EquationSystem(getDofs().size(), mode);
}

void Node::deallocateSystem()
{
    if (system)
      delete system;
}

void Node::fillin() const
{
    for (int j = 0; j < system->n; ++j) {
        for (int i = 0; i < system->n; ++i) {
            int idx;

            idx = system->index(i, j);
            system->matrix[idx] = i == j ? 1.0 : 0.0;
        }
        system->rhs[j] = 1.0;
    }
}

void Node::merge() const
{
    int i, j;
    int x, y, idx;

    int lDofsSize, lDofsToElim;
    int rDofsSize, rDofsToElim;

    lDofsSize = getLeft()->getDofs().size();
    lDofsToElim = getLeft()->getDofsToElim();

    rDofsSize = getRight()->getDofs().size();
    rDofsToElim = getRight()->getDofsToElim();

    for (j = lDofsToElim; j < lDofsSize; ++j) {
        x = leftPlaces[j - lDofsToElim];
        for (i = lDofsToElim; i < lDofsSize; ++i) {
	    y = leftPlaces[i - lDofsToElim];
	    idx = system->index(x, y);
            system->matrix[idx] = left->system->matrix[system->index(j, i)];
        }
        system->rhs[x] = left->system->rhs[j];
    }
    for (j = rDofsToElim; j < rDofsSize; ++j) {
        x = rightPlaces[j - rDofsToElim];
        for (i = rDofsToElim; i < rDofsSize; ++i) {
            y = rightPlaces[i - rDofsToElim];
	    idx = system->index(x, y);
            system->matrix[idx] += right->system->matrix[system->index(j, i)];
        }
        system->rhs[x] += right->system->rhs[j];
    }
}

void Node::eliminate() const
{
    int ret;

    if (left != NULL && right != NULL)
        merge();
    else
        fillin();

    ret = system->eliminate(getDofsToElim());
    LOG_ASSERT(ret == 0, "Elimination failed at node: %d.", node); 
}

void Node::bs() const
{
}

void Node::loadMatrix(double *data)
{
    size_t nbytes;

    nbytes = system->n * system->n;
    memcpy((void *)system->matrix, (void *)data, nbytes);
}

/* DEBUG*/

int Node::treeSize(){
    int ret = 1;

    if (getLeft() != NULL)
    	    ret += getLeft()->treeSize();

    if (getRight() != NULL)
        ret += getRight()->treeSize();

    return (ret);
}

unsigned long Node::getSizeInMemory(bool recursive)
{
    unsigned long total = (dofs.size() + 1) *
        dofs.size() * sizeof(double);
    if (recursive && left != NULL && right != NULL) {
        total += left->getSizeInMemory() + right->getSizeInMemory();
    }
    return (total);
}

unsigned long Node::getFLOPs(bool recursive)
{
    auto flops = [](unsigned int a, unsigned int b) {
        return (a * (6*b*b - 6*a*b + 6*b + 2*a*a - 3*a + 1) / 6);
    };

    unsigned long total = flops(this->getDofsToElim(), this->getDofs().size());
    if (recursive && left != NULL && right != NULL) {
        total += left->getFLOPs() + right->getFLOPs();
    }

    return (total);
}

unsigned long Node::getMemoryRearrangements()
{
    unsigned long total = 0;

    if (left != NULL && right != NULL) {
        unsigned long memLeft = (left->getDofs().size() - left->getDofsToElim());
        memLeft *= memLeft;
        unsigned long memRight = (right->getDofs().size() - right->getDofsToElim());
        memRight *= memRight;

        total = memLeft + memRight + left->getMemoryRearrangements() + right->getMemoryRearrangements();
    }

    return (total);
}


/*END OF DEBUG*/
