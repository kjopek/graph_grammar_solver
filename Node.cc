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
