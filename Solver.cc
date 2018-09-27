#include "Solver.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <tuple>


namespace solver {

Solver::Solver(std::string treePath, enum TreeFileType treeType,
    enum CBLAS_ORDER dataOrder)
{
	
	treeType = treeType;
	dataOrder = dataOrder;
	if (treeType == kTreePlain) {
		loadNodes(treePath);
	} else {
		loadCachedNodes(treePath);
	}
}

void Solver::loadCachedNodes(std::string treePath)
{
}

void Solver::loadNodes(std::string treePath)
{
	int supernodesCount;
	int treeNodesCount;
	int elementsCount;

	std::fstream treeStream(treePath, treeStream.in);
	std::map<std::tuple<int, int>, std::vector<int>> elementsMap;
	std::vector<int> supernodesCumulativeCount;

	treeStream >> supernodesCount;

	LOG_ASSERT(supernodesCount > 0, "Supernodes count must be positive.");

	supernodesCumulativeCount[0] = 0;
	for (int ii = 0; ii < supernodesCount; ii++) {
		int id, count, supernode;

		treeStream >> id;
		treeStream >> count;

		LOG_ASSERT(id >= 1,
		    "Dofs enumeration isn't 1-based (Fortran style).");
	        LOG_ASSERT(id - 1 == ii, "Node list is not ordered.");

		supernode = id - 1;
		supernodes[supernode] = count;
		supernodesCumulativeCount[supernode + 1] =
		    supernodesCumulativeCount[supernode] + count;
	}

	treeStream >> elementsCount;
	for (int ii = 0; ii < elementsCount; ii++) {
		int level, seqNo;
		int supernodesPerElement;

		treeStream >> level;
		treeStream >> seqNo;
		treeStream >> supernodesPerElement;

	        LOG_ASSERT(level >= 1,
	            "Level enumeration isn't 1-based (Fortran style).");
	        LOG_ASSERT(seqNo >= 1,
	            "Sequence number isn't 1-based (Fortran style).");
	        LOG_ASSERT(supernodesPerElement >= 1,
		    "Invalid number of supernodes per element.");

		for (; supernodesPerElement > 0; supernodesPerElement--) {
			int supernode;

			treeStream >> supernode;
			elementsMap[std::make_tuple(level, seqNo)]
				.push_back(supernode);
		}
	}

	treeStream >> treeNodesCount;

	LOG_ASSERT(treeNodesCount > 0,
	    "Number of nodes in tree must be positive.");

	nodes.resize(treeNodesCount);
	for (; treeNodesCount > 0; treeNodesCount--) {
		int id, nodeOffset, elementsCount, level, seqNo;

		treeStream >> id;
		treeStream >> elementsCount;
		
		LOG_ASSERT(id >= 1,
		    "Node enumeration isn't 1-based (Fortran style).");
		LOG_ASSERT(elementsCount >= 1,
		    "Invalid number of elements in node.");

		Node node(id);
		nodeOffset = id - 1;

		for (int ii = 0; ii < elementsCount; ii++) {
			/* Ignore */
			treeStream >> level;
			treeStream >> seqNo;
		}

		if (elementsCount > 1) {
			int leftSon, rightSon;

			treeStream >> leftSon;
			treeStream >> rightSon;

			LOG_ASSERT(leftSon >= 1,
			    "Left son isn't 1-based (Fortran style).");
			LOG_ASSERT(rightSon >= 1,
			    "Right son isn't 1-based (Fortran style).");

			node.setLeftId(leftSon);
			node.setRightId(rightSon);
		} else {
			node.setElementDofs(elementsMap[
				std::make_tuple(level, seqNo)
			]);
		}
		nodes[nodeOffset] = node;
	}
	treeStream.close();
}

void Solver::analyseTree()
{
	Node &root = getNode(0);
	std::set<int> parent;

	nodeAnaliser(root, parent);
	mergeAnaliser(root);
}

void Solver::nodeAnaliser(Node &node, std::set<int> &parent)
{
	auto getAllDOFs = [] (Node &node, std::set<int> &dofs) {
		std::vector<int> &elementDofs = node.getElementDofs();
		dofs.insert(elementDofs.begin(), elementDofs.end());
	};

	std::set<int> common;

	if (node.getLeftId() != -1 && node.getRightId() != -1) {
		std::set<int> lDofs;
		std::set<int> rDofs;

		Node &leftNode = getNode(node.getLeftId());
		Node &rightNode = getNode(node.getRightId());

		getAllDOFs(leftNode, lDofs);
		getAllDOFs(rightNode, rDofs);

		std::set_intersection(lDofs.begin(), lDofs.end(),
		    rDofs.begin(), rDofs.end(),
		    std::inserter(common, common.begin()));

		for (auto p = parent.cbegin(); p != parent.cend(); ++p) {
			if (lDofs.count(*p) || rDofs.count(*p)) {
				common.insert(*p);
			}
		}

		lDofs.clear();
		rDofs.clear();

		nodeAnaliser(leftNode, common);
		nodeAnaliser(rightNode, common);

	} else {
		getAllDOFs(node, common);
	}

	int i = 0;

	for (auto dof: common) {
		if (!parent.count(dof)) {
			node.addDof(dof);
			++i;
		}
	}

	node.setElimSize(i);

	for (auto dof: common) {
		if (parent.count(dof)) {
			node.addDof(dof);
		}
	}
}

void Solver::mergeAnaliser(Node &node)
{
	if (node.getLeftId() != -1 && node.getRightId() != -1) {
		int leftDofsSize, leftDofsElim;
		int rightDofsSize, rightDofsElim;

		Node &leftNode = getNode(node.getLeftId());
		Node &rightNode = getNode(node.getRightId());

		std::map<int, int> reverseMap;

		leftDofsSize = leftNode.getDofs().size();
		leftDofsElim = leftNode.getElimSize();
		rightDofsSize = rightNode.getDofs().size();
		rightDofsElim = rightNode.getDofs().size();

		node.leftPlaces.resize(leftDofsSize - leftDofsElim);
		node.rightPlaces.resize(rightDofsSize - rightDofsElim);

		for (int i = 0; i < node.getDofs().size(); ++i)
			reverseMap[node.getDofs()[i]] = i;

		for (int i = leftDofsElim; i < leftDofsSize; ++i) {
			node.leftPlaces[i - leftDofsElim] =
				reverseMap[leftNode.getDofs()[i]];
		}

		for (int i = rightDofsElim; i < rightDofsSize; ++i) {
			node.rightPlaces[i - rightDofsElim] =
				reverseMap[rightNode.getDofs()[i]];
		}

		mergeAnaliser(leftNode);
		mergeAnaliser(rightNode);
	}
}

}
