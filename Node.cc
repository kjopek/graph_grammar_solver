#include <algorithm>
#include <set>
#include <string.h>

#include "Node.h"
#include "log.h"

int Node::getId() const
{
	return (id);
}

int Node::getParentId() const
{
	return (parentId);
}

int Node::getLeftId() const
{
	return (leftId);
}

int Node::getRightId() const
{
	return (rightId);
}

void Node::setParentId(int parentId)
{
	parentId = parentId;
}

void Node::setLeftId(int leftId)
{
	leftId = leftId;
}

void Node::setRightId(int rightId)
{
	rightId = rightId;
}

void Node::addDof(int dof)
{
	dofs.push_back(dof);
}

std::vector<int> &Node::getDofs()
{
	return (dofs);
}

void Node::clearDofs()
{
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

void Node::setElimSize(int size)
{
	elimSize = size;
}

int Node::getElimSize() const
{
	return (elimSize);
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
            system->matrix[idx] = left->system->matrix[left->system->index(j, i)];
        }
        system->rhs[x] = left->system->rhs[j];
    }
    for (j = rDofsToElim; j < rDofsSize; ++j) {
        x = rightPlaces[j - rDofsToElim];
        for (i = rDofsToElim; i < rDofsSize; ++i) {
            y = rightPlaces[i - rDofsToElim];
	    idx = system->index(x, y);
            system->matrix[idx] += right->system->matrix[right->system->index(j, i)];
        }
        system->rhs[x] += right->system->rhs[j];
    }
}

void Node::eliminate() const
{
    int ret;

    if (left != NULL && right != NULL)
        merge();

    ret = system->eliminate(getDofsToElim());
    LOG_ASSERT(ret == 0, "Elimination failed at node: %d.", id); 
}

void Node::bs() const
{
}

void Node::loadMatrix(double *data)
{
    size_t nbytes;

    nbytes = system->n * system->n * sizeof(*system->matrix);
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
