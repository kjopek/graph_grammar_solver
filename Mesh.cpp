#include "Mesh.hpp"
#include "log.h"
#include <errno.h>
#include <set>
#include <map>
#include <tuple>
#include <algorithm>

void Mesh::addNode(Node *n)
{
    nodes.push_back(n);
}

Node *Mesh::getRootNode()
{
    return nodes[0];
}

void Mesh::setDofs(int dofs)
{
    dofs = dofs;
}

int Mesh::getDofs()
{
    return (dofs);
}

Mesh *Mesh::loadFromFile(const char *filename)
{
    /*
     * This procedure is not optimized and could be (together with analysis)
     * heavily cached. Has yet to be done.
     */
    FILE *fp;
    Mesh *mesh;
    int ndofs, nelements, nodes;
    std::vector<int> ndofsFold;
    std::map<std::tuple<int, int>, std::vector<int>> elementToDofs;

    fp = fopen(filename, "r");
    LOG_ASSERT(fp != NULL, "Failed to open file: %d.", errno);

    fscanf(fp, "%d", &ndofs);
    LOG_ASSERT(ndofs > 0, "Negative number of DOFs.");
    for (int ii = 0; ii < ndofs; ii++) {
        int id, temp;

	fscanf(fp, "%d%d", &id, &temp);

	LOG_ASSERT(id >= 1, "Dofs enumeration isn't 1-based (Fortran style).");
	LOG_ASSERT(id - 1 == ii, "Node list is not ordered.");
	LOG_ASSERT(temp == 1, "Supernodes with node count > 1 are not supported now.");
	ndofsFold.push_back(temp);
    }

    fscanf(fp, "%d", &nelements);
    LOG_ASSERT(nelements >= 1, "Mesh must contain more than 1 element.");

    for (int ii = 0; ii < nelements; ii++) {
        int level, seqno;
        int dofsOfElement;

	std::vector<int> dofs;

        fscanf(fp, "%d%d%d", &level, &seqno, &dofsOfElement);
	LOG_ASSERT(level >= 1,
            "Level enumeration isn't 1-based (Fortran style).");
	LOG_ASSERT(seqno >= 1,
            "Sequence number isn't 1-based (Fortran style).");
	LOG_ASSERT(dofsOfElement >= 1, "Invalid number of DOFs per element.");

        for (; dofsOfElement > 0; dofsOfElement--) {
            int dof;

            fscanf(fp, "%d", &dof);
            elementToDofs[std::make_tuple(level, seqno)].push_back(dof);
        }
    }

    mesh = new Mesh();

    std::vector<Node *> nodesVector;

    fscanf(fp, "%d", &nodes);
    nodesVector.resize(nodes);

    for (int i = 0; i < nodes; ++i) {
        int nodeId, nodeOffset;
        int nrElems;
        int level, seqno;

	fscanf(fp, "%d %d", &nodeId, &nrElems);
        LOG_ASSERT(nodeId >= 1,
            "Node enumeration isn't 1-based (Fortran style).");
        LOG_ASSERT(nrElems >= 1, "Invalid number of elements in node.");

        Node *node = new Node(nodeId);
	nodeOffset = nodeId - 1;
        nodesVector[nodeOffset] = node;

	for (int jj = 0; jj < nrElems; jj++)
            fscanf(fp, "%d%d", &level, &seqno);

        if (nrElems > 1) {
            int leftSon, rightSon;

	    fscanf(fp, "%d%d", &leftSon, &rightSon);
	    LOG_ASSERT(leftSon >= 1,
                "Left son isn't 1-based (Fortran style).");
            LOG_ASSERT(rightSon >= 1,
                "Right son isn't 1-based (Fortran style).");

	    node->setLeft(nodesVector[leftSon - 1]);
            node->setRight(nodesVector[rightSon - 1]);
        } else {
            node->setElementDofs(elementToDofs[std::make_tuple(level, seqno)]);
	}
    }

    for (int ii = 0; ii < nodes; ii++) {
        mesh->addNode(nodesVector[ii]);
    }

    fclose(fp);
    return (mesh);
}

