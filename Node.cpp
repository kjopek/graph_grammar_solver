#include "Node.hpp"
#include <set>
#include <algorithm>
#include "Analysis.hpp"

void Node::setLeft(Node *left)
{
    this->left = left;
}

void Node::setRight(Node *right)
{
    this->right = right;
}

void Node::setParent(Node *parent)
{
    this->parent = parent;
}

void Node::addElement(Element *e)
{
    this->mergedElements.push_back(e);
}

void Node::clearElements(){
    this->mergedElements.clear();
}

void Node::setProduction(std::string &prodname)
{
    this->production = prodname;
}

std::string &Node::getProduction()
{
    return this->production;
}

Node *Node::getLeft() const
{
    return this->left;
}

Node *Node::getRight() const
{
    return this->right;
}

Node *Node::getParent() const
{
    return this->parent;
}

std::vector<Element *> &Node::getElements()
{
    return this->mergedElements;
}

int Node::getId() const
{
    return this->node;
}

void Node::addDof(uint64_t dof)
{
    this->dofs.push_back(dof);
}

std::vector<uint64_t> &Node::getDofs()
{
    return this->dofs;
}

void Node::clearDofs(){
    this->dofs.clear();
}

void Node::setDofsToElim(uint64_t dofs)
{
    this->dofsToElim = dofs;
}

uint64_t Node::getDofsToElim() const
{
    return this->dofsToElim;
}

void Node::allocateSystem(SolverMode mode)
{
    this->system = new EquationSystem(this->getDofs().size(), mode);
}

void Node::deallocateSystem()
{
    if (this->system)
      delete this->system;
}

void Node::fillin() const
{
    for (int j = 0; j < system->n; ++j) {
        for (int i = 0; i < system->n; ++i) {
            this->system->matrix[j][i] = i == j ? 1.0 : 0.0;
        }
        this->system->rhs[j] = 1.0;
    }
}

void Node::merge() const
{
    int i, j;
    int x, y;

    for (j = getLeft()->getDofsToElim(); j < getLeft()->getDofs().size(); ++j) {
        for (i = getLeft()->getDofsToElim(); i < getLeft()->getDofs().size(); ++i) {
            system->matrix[leftPlaces[j - getLeft()->getDofsToElim()]][leftPlaces[i - getLeft()->getDofsToElim()]] =
                    left->system->matrix[j][i];
        }
        system->rhs[leftPlaces[j-getLeft()->getDofsToElim()]] = left->system->rhs[j];
    }
    for (j=getRight()->getDofsToElim(); j<getRight()->getDofs().size(); ++j) {
        for (i=getRight()->getDofsToElim(); i<getRight()->getDofs().size(); ++i) {
            system->matrix[rightPlaces[j-getRight()->getDofsToElim()]][rightPlaces[i-getRight()->getDofsToElim()]] +=
                    right->system->matrix[j][i];
        }
        system->rhs[rightPlaces[j-getRight()->getDofsToElim()]] += right->system->rhs[j];
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
    if (this->getLeft() != NULL){
        ret += this->getLeft()->treeSize();
    }
    if (this->getRight() != NULL){
        ret += this->getRight()->treeSize();
    }
    return ret;
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

        total = memLeft+memRight+left->getMemoryRearrangements()+right->getMemoryRearrangements();
    }

    return total;
}


/*END OF DEBUG*/
