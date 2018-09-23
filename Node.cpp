#include "Node.hpp"
#include <set>
#include <algorithm>
#include "Analysis.hpp"

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

void Node::addElement(Element *e)
{
    mergedElements.push_back(e);
}

void Node::clearElements(){
    mergedElements.clear();
}

void Node::setProduction(std::string &prodname)
{
    production = prodname;
}

std::string &Node::getProduction()
{
    return (production);
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

std::vector<Element *> &Node::getElements()
{
    return (mergedElements);
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
            system->matrix[j][i] = i == j ? 1.0 : 0.0;
        }
        system->rhs[j] = 1.0;
    }
}

void Node::merge() const
{
    int i, j;
    int x, y;

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
            system->matrix[x][y] = left->system->matrix[j][i];
        }
        system->rhs[x] = left->system->rhs[j];
    }
    for (j = rDofsToElim; j < rDofsSize; ++j) {
        x = rightPlaces[j - rDofsToElim];
        for (i = rDofsToElim; i < rDofsSize; ++i) {
            y = rightPlaces[i - rDofsToElim];
            system->matrix[x][y] += right->system->matrix[j][i];
        }
        system->rhs[x] += right->system->rhs[j];
    }
}

void Node::eliminate() const
{
    if (left != NULL && right != NULL) {
        merge();
    } else {
        fillin();
    }
    system->eliminate(getDofsToElim());
}

void Node::bs() const
{
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
        dofs.size() * sizeeof(double);
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
