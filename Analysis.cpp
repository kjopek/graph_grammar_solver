#include "Analysis.hpp"
#include <algorithm>

using namespace std;

void Analysis::findLeftRotationChild(Node ** _parent, Node ** _root, bool child1, bool child2, Node ** _child, Node ** _otherChild, Mesh * mesh){
    Node * parent = (*_parent);
    Node * root = (*_root);
    
    Node * child;
    Node * otherChild;
    std::vector<Element *> allElements = mesh->getElements();
    
    if (root->getRight()->getLeft() == NULL){
        (*_child) = root->getRight()->getRight();
        (*_otherChild) = root->getRight()->getLeft();
        return;
    }
    
    
    if (root->getRight()->getLeft() == NULL){
        (*_child) = root->getRight()->getLeft();
        (*_otherChild) = root->getRight()->getRight();
        return;
    }
    
    if (child1){
        if (child2){
            if (root->getRight()->getLeft()->getNumberOfNeighbours(allElements) < root->getRight()->getRight()->getNumberOfNeighbours(allElements)){
                child = root->getRight()->getLeft();
                otherChild = root->getRight()->getRight();
            } else {
                child = root->getRight()->getRight();
                otherChild = root->getRight()->getLeft();
            }
        } else{
            child = root->getRight()->getLeft();
            otherChild = root->getRight()->getRight();
        }
    } else{
        child = root->getRight()->getRight();
        otherChild = root->getRight()->getLeft();
    }
    
    (*_child) = child;
    (*_otherChild) = otherChild;
}


void Analysis::findRightRotationChild(Node ** _parent, Node ** _root, bool child1, bool child2, Node ** _child, Node ** _otherChild, Mesh * mesh){
    Node * parent = (*_parent);
    Node * root = (*_root);
    
    Node * child;
    Node * otherChild;
    std::vector<Element *> allElements = mesh->getElements();
    
    if (root->getLeft()->getLeft() == NULL){
        (*_child) = root->getLeft()->getRight();
        (*_otherChild) = root->getLeft()->getLeft();
        return;
    }
    
    
    if (root->getLeft()->getLeft() == NULL){
        (*_child) = root->getLeft()->getLeft();
        (*_otherChild) = root->getLeft()->getRight();
        return;
    }
    
    if (child1){
        if (child2){
            if (root->getLeft()->getLeft()->getNumberOfNeighbours(allElements) < root->getLeft()->getRight()->getNumberOfNeighbours(allElements)){
                child = root->getLeft()->getLeft();
                otherChild = root->getLeft()->getRight();
            } else {
                child = root->getLeft()->getRight();
                otherChild = root->getLeft()->getLeft();
            }
        } else{
            child = root->getLeft()->getLeft();
            otherChild = root->getLeft()->getRight();
        }
    } else{
        child = root->getLeft()->getRight();
        otherChild = root->getLeft()->getLeft();
    }
    
    (*_child) = child;
    (*_otherChild) = otherChild;
}


Node * Analysis::leftRotation(Node ** _parent, Node ** _root, bool child1, bool child2, Node ** _child, Node ** _otherChild, bool * b){
    
    Node * parent = (*_parent);
    Node * root = (*_root);
    Node * child = (*_child);
    Node * otherChild = (*_otherChild);
    Node * T = root->getRight();
    Node * t = root->getLeft();
    
    if (T==NULL){
        return root;
    }
    
    root->setRight(t);
    root->setLeft(child);
    T->setRight(otherChild);
    T->setLeft(root);
    
    if (parent != NULL){
        if (parent->getLeft() == root){
            parent->setLeft(T);
        } else {
            parent->setRight(T);
        }
    }
    
    T->rebuildElements();
    root->rebuildElements();
    (*b)=false;
    
    return T;
}


Node * Analysis::rightRotation(Node ** _parent, Node ** _root, bool child1, bool child2, Node ** _child, Node ** _otherChild, bool * b){
    
    Node * parent = (*_parent);
    Node * root = (*_root);
    Node * child = (*_child);
    Node * otherChild = (*_otherChild);
    Node * T = root->getLeft();
    Node * t = root->getRight();
    
    if (T==NULL){
        return root;
    }
    
    root->setLeft(t);
    root->setRight(child);
    T->setLeft(otherChild);
    T->setRight(root);
    
    if (parent != NULL){
        if (parent->getLeft() == root){
            parent->setLeft(T);
        } else {
            parent->setRight(T);
        }
    }
    
    T->rebuildElements();
    root->rebuildElements();
    (*b)=false;
    
    return T;
}

int Analysis::rotate(Node * root, Node * parent, Mesh * mesh, bool * b){
    
    int h;
    
    if (root->getLeft() != NULL) {
        root->l = Analysis::rotate(root->getLeft(), root, mesh, b);
    } else {
        root->l=0;
    }
    
    if (root->getRight() != NULL) {
        root->r = Analysis::rotate(root->getRight(), root, mesh, b);
    } else {
        root->r=0;
    }
    h=root->r-root->l;

    if ( (h==1) || (h==-1) || (h==0) ) { //no rotation
        if (root->l>root->r){
            return root->l+1;
        } else {
            return root->r+1;
        }
    }
    
    Node * T = root;
    
    if (h>=2) {  //we need to perform some rotations to the left
        bool child1;
        bool child2;
        Node * child;
        Node * otherChild;
        
        if (parent  == NULL){
            child1 = true;
            child2 = true;
        } else {
            child1 = Node::isNeighbour(root->getRight()->getLeft(), parent->getLeft());
            child2 = Node::isNeighbour(root->getRight()->getRight(), parent->getLeft());
        }
        findLeftRotationChild(&parent, &root, child1, child2, &child, &otherChild, mesh);
        
        if (((child == root->getRight()->getRight()) && (root->getRight()->r<=root->getRight()->l))
                || ((child == root->getRight()->getLeft()) && (root->getRight()->l<=root->getRight()->r))) {//rotation to the left
            T = leftRotation(&parent, &root, child1, child2, &child, &otherChild, b);
            //printf("rotation left\n");
        } //end of rotation to the left
        else //double rotation
        {// lower level rotation to the right (rotation at root->right)
            Node * parent1 = root;
            Node * root1 = root->getRight();
            bool nchild1;
            bool nchild2;
            Node * nchild;
            Node * notherChild;
            
            if (parent1  == NULL){
                nchild1 = true;
                nchild2 = true;
            } else {
                nchild1 = Node::isNeighbour(root1->getLeft()->getLeft(), parent1->getRight());
                nchild2 = Node::isNeighbour(root1->getLeft()->getRight(), parent1->getRight());
            }
            findRightRotationChild(&parent1, &root1, nchild1, nchild2, &nchild, &notherChild, mesh);
            rightRotation(&parent1, &root1, nchild1, nchild2, &nchild, &notherChild, b);
            
            //rotation on higher level in left
            if (parent  == NULL){
                child1 = true;
                child2 = true;
            } else {
                child1 = Node::isNeighbour(root->getRight()->getLeft(), parent->getLeft());
                child2 = Node::isNeighbour(root->getRight()->getRight(), parent->getLeft());
            }
            findLeftRotationChild(&parent, &root, child1, child2, &child, &otherChild, mesh);
            
            T = leftRotation(&parent, &root, child1, child2, &child, &otherChild, b);
            //printf("double rotation left\n");
        }//end of double rotations
    }// end if (h>=2)
    if (h<=-2) {  // the same, but other direction rotation
        bool child1;
        bool child2;
        Node * child;
        Node * otherChild;
        
        if (parent  == NULL){
            child1 = true;
            child2 = true;
        } else {
            child1 = Node::isNeighbour(root->getLeft()->getLeft(), parent->getRight());
            child2 = Node::isNeighbour(root->getLeft()->getRight(), parent->getRight());
        }
        findRightRotationChild(&parent, &root, child1, child2, &child, &otherChild, mesh);
        
        if (((child == root->getLeft()->getRight()) && (root->getLeft()->r<=root->getLeft()->l))
                || ((child == root->getLeft()->getLeft()) && (root->getLeft()->l<=root->getLeft()->r))) {//rotation to the left
            T = rightRotation(&parent, &root, child1, child2, &child, &otherChild, b);
            //printf("rotation right\n");
        }//end of rotation to the right
        
        else //double rotation
        {// lower level rotation to the left
            //   (rotation at root->left)
            Node * parent1 = root;
            Node * root1 = root->getLeft();
            bool nchild1;
            bool nchild2;
            Node * nchild;
            Node * notherChild;
            
            if (parent1  == NULL){
                nchild1 = true;
                nchild2 = true;
            } else {
                nchild1 = Node::isNeighbour(root1->getRight()->getLeft(), parent1->getLeft());
                nchild2 = Node::isNeighbour(root1->getRight()->getRight(), parent1->getLeft());
            }
            findLeftRotationChild(&parent1, &root1, nchild1, nchild2, &nchild, &notherChild, mesh);
            
            leftRotation(&parent1, &root1, nchild1, nchild2, &nchild, &notherChild, b);
            if (parent  == NULL){
                child1 = true;
                child2 = true;
            } else {
                child1 = Node::isNeighbour(root->getLeft()->getLeft(), parent->getRight());
                child2 = Node::isNeighbour(root->getLeft()->getRight(), parent->getRight());
            }
            findRightRotationChild(&parent, &root, child1, child2, &child, &otherChild, mesh);
            
            T = rightRotation(&parent, &root, child1, child2, &child, &otherChild, b);
            //printf("double rotation right\n");
        }//end of double rotations
    }
    
    if (parent == NULL){
        mesh->setRootNode(T);
    }
    
    std::set<Node*>a;
    if (T->isLooped(&a)){
        printf("looped tree\n");
        exit(-1);
    }
    bool bl=true;
    T->isBalanced(&bl);
    if (root->l>T->r){
        return T->l+1;
    } else {
        return T->r+1;
    }
}

void Analysis::nodeAnaliser(Node *node, set<uint64_t> *parent)
{
    auto getAllDOFs = [] (Node *n) {
        set<uint64_t> *dofs = new set<uint64_t>();
        for (Element *e : n->getElements()) {
            for (uint64_t dof : e->dofs)
                dofs->insert(dof);
        }
        return dofs;
    };

    set<uint64_t> *common;

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        set<uint64_t> *lDofs = getAllDOFs(node->getLeft());
        set<uint64_t> *rDofs = getAllDOFs(node->getRight());

        common = new set<uint64_t>;
        std::set_intersection(lDofs->begin(), lDofs->end(),
                              rDofs->begin(), rDofs->end(),
                              std::inserter(*common, common->begin()));

        delete (lDofs);
        delete (rDofs);

        for (auto p = parent->cbegin(); p!=parent->cend(); ++p) {
            common->insert(*p);
        }

        Analysis::nodeAnaliser(node->getLeft(), common);
        Analysis::nodeAnaliser(node->getRight(), common);

    } else {
        common = getAllDOFs(node);
    }

    int i = 0;

    for (uint64_t dof : *common) {
        if (!parent->count(dof)) {
            node->addDof(dof);
            ++i;
        }
    }

    node->setDofsToElim(i);

    for (uint64_t dof : *common) {
        if (parent->count(dof)) {
            node->addDof(dof);
        }
    }

    delete common;
}

void Analysis::doAnalise(Mesh *mesh)
{
    Node *root = mesh->getRootNode();
    std::set<uint64_t> *parent = new set<uint64_t>();
    Analysis::nodeAnaliser(root, parent);
    Analysis::mergeAnaliser(root);

    delete parent;
}

void Analysis::mergeAnaliser(Node *node)
{
    if (node->getLeft() != NULL && node->getRight() != NULL) {
        node->leftPlaces = new uint64_t[node->getLeft()->getDofs().size() - node->getLeft()->getDofsToElim()];
        node->rightPlaces = new uint64_t[node->getRight()->getDofs().size() - node->getRight()->getDofsToElim()];

        map<uint64_t, uint64_t> reverseMap;

        for (int i=0; i<node->getDofs().size(); ++i) {
            reverseMap[node->getDofs()[i]] = i;
        }

        for (int i=node->getLeft()->getDofsToElim(); i<node->getLeft()->getDofs().size(); ++i) {
            node->leftPlaces[i-node->getLeft()->getDofsToElim()] = reverseMap[node->getLeft()->getDofs()[i]];
        }

        for (int i=node->getRight()->getDofsToElim(); i<node->getRight()->getDofs().size(); ++i) {
            node->rightPlaces[i-node->getRight()->getDofsToElim()] = reverseMap[node->getRight()->getDofs()[i]];
        }

        Analysis::mergeAnaliser(node->getLeft());
        Analysis::mergeAnaliser(node->getRight());
    }
}

tuple<edge, uint64_t> Analysis::parentEdge(edge e,
                                          std::map<uint64_t, std::map<vertex, uint64_t> > &levelVertices,
                                          std::map<uint64_t, std::map<edge, uint64_t> > &levelEdges,
                                          uint64_t level)
{
    // this function returns either parent egde (if exists)
    // or the specified edge if there is no parent edge
    vertex &v1 = std::get<0>(e);
    vertex &v2 = std::get<1>(e);
    uint64_t x1 = std::get<0>(v1);
    uint64_t y1 = std::get<1>(v1);
    uint64_t x2 = std::get<0>(v2);
    uint64_t y2 = std::get<1>(v2);

    // horizontal edge
    if (y1 == y2) {
        if (levelEdges[level-1].count(edge(v1,vertex(2*x2-x1, y1)))) {
            return tuple<edge, uint64_t>(edge(v1,vertex(2*x2-x1, y1)), 2);
        }
        if (levelEdges[level-1].count(edge(vertex(2*x1-x2, y1), v2))) {
            return tuple<edge, uint64_t>(edge(vertex(2*x1-x2, y1), v2), 1);
        }
        return tuple<edge, uint64_t>(e, 0);
    } else {
        if (levelEdges[level-1].count(edge(v1,vertex(x1, 2*y2-y1)))) {
            return tuple<edge, uint64_t>(edge(v1,vertex(x1, 2*y2-y1)), 2);
        }
        if (levelEdges[level-1].count(edge(vertex(x1, 2*y1-y2), v2))) {
            return tuple<edge, uint64_t>(edge(vertex(x1, 2*y1-y2), v2), 1);
        }
        return tuple<edge, uint64_t>(e, 0);
    }
}

void Analysis::enumerateElem(Mesh *mesh, Element *elem,
                             std::map<uint64_t, std::map<vertex, uint64_t> > &levelVertices,
                             std::map<uint64_t, std::map<edge, uint64_t> > &levelEdges,
                             uint64_t &n, uint64_t level)
{
    map<vertex, uint64_t> &vertices = levelVertices[level];
    map<edge, uint64_t> &edges = levelEdges[level];

    edge e1(vertex(elem->x1, elem->y1), vertex(elem->x2, elem->y1));
    edge e2(vertex(elem->x2, elem->y1), vertex(elem->x2, elem->y2));
    edge e3(vertex(elem->x1, elem->y2), vertex(elem->x2, elem->y2));
    edge e4(vertex(elem->x1, elem->y1), vertex(elem->x1, elem->y2));

    // for other layers we need to take into consideration also
    // h-adaptation and its influence on positions of DOF
    map<vertex, uint64_t> &parentVertices = levelVertices[level-1];
    map<edge, uint64_t> &parentEdges = levelEdges[level-1];

    tuple<edge, uint64_t> ve1 = Analysis::parentEdge(e1, levelVertices, levelEdges, level);
    tuple<edge, uint64_t> ve2 = Analysis::parentEdge(e2, levelVertices, levelEdges, level);
    tuple<edge, uint64_t> ve3 = Analysis::parentEdge(e3, levelVertices, levelEdges, level);
    tuple<edge, uint64_t> ve4 = Analysis::parentEdge(e4, levelVertices, levelEdges, level);


    vertex v1(std::min(std::get<0>(std::get<0>(std::get<0>(ve1))),
                       std::get<0>(std::get<0>(std::get<0>(ve4)))),
              std::min(std::get<1>(std::get<0>(std::get<0>(ve1))),
                       std::get<1>(std::get<0>(std::get<0>(ve4)))));
    vertex v2(std::max(std::get<0>(std::get<1>(std::get<0>(ve1))),
                       std::get<0>(std::get<0>(std::get<0>(ve2)))),
              std::min(std::get<1>(std::get<1>(std::get<0>(ve1))),
                       std::get<1>(std::get<0>(std::get<0>(ve2)))));
    vertex v3(std::max(std::get<0>(std::get<1>(std::get<0>(ve2))),
                       std::get<0>(std::get<1>(std::get<0>(ve3)))),
              std::max(std::get<1>(std::get<1>(std::get<0>(ve2))),
                       std::get<1>(std::get<1>(std::get<0>(ve3)))));
    vertex v4(std::min(std::get<0>(std::get<0>(std::get<0>(ve3))),
                       std::get<0>(std::get<1>(std::get<0>(ve4)))),
              std::max(std::get<1>(std::get<0>(std::get<0>(ve3))),
                       std::get<1>(std::get<1>(std::get<0>(ve4)))));

    auto add_vertex = [&] (vertex &v) {
        if (parentVertices.count(v)) {
            vertices[v] = parentVertices[v];
        } else {
            if (!vertices.count(v)) {
                vertices[v] = n++;
            }
        }
        elem->dofs.push_back(vertices[v]);
    };

    auto add_edge = [&] (edge &e) {
        if (parentEdges.count(e)) {
            edges[e] = parentEdges[e];
        } else {
            if (!edges.count(e)) {
                edges[e] = n;
                n += mesh->getPolynomial()-1;
            }
        }
        for (uint64_t i=0; i<(mesh->getPolynomial()-1); ++i) {
            elem->dofs.push_back(edges[e]+i);
        }
    };

    add_vertex(v1);
    add_vertex(v2);
    add_vertex(v3);
    add_vertex(v4);

    add_edge(std::get<0>(ve1));
    add_edge(std::get<0>(ve2));
    add_edge(std::get<0>(ve3));
    add_edge(std::get<0>(ve4));

    for (uint64_t i=0; i<(mesh->getPolynomial()-1)*(mesh->getPolynomial()-1); ++i) {
        elem->dofs.push_back(n+i);
    }
    n += (mesh->getPolynomial()-1)*(mesh->getPolynomial()-1);
}

void Analysis::enumerateElem1(Mesh *mesh, Element *elem,
                             map<uint64_t, map<vertex, uint64_t>> &levelVertices,
                             map<uint64_t, map<edge, uint64_t>> &levelEdges,
                             uint64_t &n)
{
    map<vertex, uint64_t> &vertices = levelVertices[1];
    map<edge, uint64_t> &edges = levelEdges[1];

    vertex v1(elem->x1, elem->y1);
    vertex v2(elem->x2, elem->y1);
    vertex v3(elem->x2, elem->y2);
    vertex v4(elem->x1, elem->y2);

    edge e1(v1, v2);
    edge e2(v2, v3);
    edge e3(v4, v3);
    edge e4(v1, v4);

    auto add_vertex = [&] (vertex &v) {
        if (!vertices.count(v)) {
            vertices[v] = n++;
        }
        elem->dofs.push_back(vertices[v]);
    };

    auto add_edge = [&] (edge &e) {
        if (!edges.count(e)) {
            edges[e] = n;
            n += mesh->getPolynomial()-1;
        }
        for (uint64_t i=0; i<mesh->getPolynomial()-1; ++i) {
            elem->dofs.push_back(edges[e]+i);
        }
    };

    // vertices
    add_vertex(v1);
    add_vertex(v2);
    add_vertex(v3);
    add_vertex(v4);

    // edges
    add_edge(e1);
    add_edge(e2);
    add_edge(e3);
    add_edge(e4);
    // in 2-dimensional space the faces do not overlap
    for (uint64_t i=0; i<(mesh->getPolynomial()-1)*(mesh->getPolynomial()-1); ++i) {
        elem->dofs.push_back(n+i);
    }
    n += (mesh->getPolynomial()-1)*(mesh->getPolynomial()-1);
}

void Analysis::enumerateDOF(Mesh *mesh)
{
    map<uint64_t, vector<Element*>> elementMap;
    set<uint64_t> levels;

    map<uint64_t, map<vertex, uint64_t>> levelVertices;
    map<uint64_t, map<edge, uint64_t>> levelEdges;

    uint64_t n = 1;

    auto compare = [](Element *e1, Element *e2) {
        if (e1->k > e2->k) {
            return false;
        } else if (e1->k < e2->k) {
            return true;
        }

        if (e1->l < e2->l) {
            return true;
        } else if (e1->l > e2->l) {
            return false;
        }
        return false;
    };

    std::sort(mesh->getElements().begin(), mesh->getElements().end(), compare);

    // now, we have level plan for mesh
    for (Element *e : mesh->getElements()) {
        levels.insert(e->k);
        elementMap[e->k].push_back(e);
    }

    // implementation assumes that the neighbours may vary on one level only
    for (uint64_t level : levels) {
        vector<Element *> elems = elementMap[level];
        // on the first layer we do not need to care about adaptation
        for (Element *elem : elems) {
            if (level == 1) {
                Analysis::enumerateElem1(mesh, elem, levelVertices, levelEdges, n);
            } else {
                Analysis::enumerateElem(mesh, elem, levelVertices, levelEdges, n, level);
            }
        }
    }
    mesh->setDofs(n-1);
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

