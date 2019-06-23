//
// Created by guihang on 2019/6/9.
//
#include "art.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

Node *ART::insert(vector<Edge *> &v) {
    if (v.empty()) return nullptr;
    Node *parent = root;
    for (auto &i : v) {
        Node *child = findChild(parent, i);
        if (!child) {
            child = new Node();
            child->data = new Edge(*i);
            child->parent = parent;
            // insert new child to the front of the children
            child->next = parent->children;
            parent->children = child;
        }
        parent = child;
    }
    return parent;
}

void ART::insert(vector<Edge *> v, Node *root, int begin) {
    if (v.size() <= begin) {
        root->leafNum++;                    //mark leaf, if node->leafNum>=1, the node is a leaf.
        return;
    }
    Node *child = nullptr;
    // if not find , insert new node to the front of the children
    if (!(child = findChild(root, v[begin]))) {
        child = new Node();
        child->data = new Edge(*v[begin]);
        child->parent = root;

        // insert new child to the front of the children
        child->next = root->children;
        root->children = child;
    }
    // node->childrenNum++;

    // insert deeper
    insert(v, child, ++begin);
}

vector<Edge *> ART::retrieve(Node *node) {
    vector<Edge *> v;
    while (node->data != nullptr) {
        v.push_back(node->data);
        node = node->parent;
    }
    return v;
}

void ART::del(Node *leaf) {
    if (leaf == nullptr)
        return;
    Node *node = leaf->parent;
    //Node* temp = node;
    if (leaf->leafNum == 1 && leaf->children == nullptr) {

        if (leaf->parent->children->equal(leaf)) {
            if (leaf->next) {                                       // 1.1.1 if the node has next
                leaf->parent->children = leaf->next;
            } else {                                                // 1.1.2 if the node does not have next
                leaf->parent->children = nullptr;
            }

            delete leaf;
        } else {
            Node *before = leaf->parent->children;                                    // before is the node before the node to be deleted
            while (!before->next->equal(leaf)) {
                before = before->next;
            }
            before->next = leaf->next;
            delete leaf;
        }
    } else {
        leaf->leafNum--;
        return;
    }

    while (node->parent && node->leafNum == 0 && !node->children) {
        Node *temp = node;
        if (node->parent->children->equal(node)) {
            if (node->next) {                                       // 1.1.1 if the node has next
                node->parent->children = node->next;
            } else {                                                // 1.1.2 if the node does not have next
                node->parent->children = nullptr;
            }
            delete node;
            node = nullptr;
        } else {
            Node *before = node;                                    // before is the node before the node to be deleted
            while (!before->next->equal(node)) {
                before = before->next;
            }
            before->next = node->next;
            delete node;
            node = nullptr;
        }
        node = temp->parent;
    }
}

void ART::DFS(Node *node) {
    if (node->children) {
        DFS(node->children);
    }
    if (node->next) {
        DFS(node->next);
    }
    //access the edge info
}

Node *ART::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    Node *children = parent->children;
    while (children) {
        if (children->equal(child)) return children;
        children = children->next;
    }
    return nullptr;
}

//PEGraph *ART::retrieve(PEGraph_Pointer graph_pointer) {
//    if (m.find(graph_pointer)!= m.end()){
//        Node *node = m[graph_pointer];
//        vector<Edge *> v = retrieve(node);
//        return convertToPEGraph(v);
//    }

//}

//void ART::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    Node *node = m[graph_pointer];
//    del(node);
//    vector<Edge *> v = convertToVector(pegraph);
//    Node *leaf = insert(v);
//    m[graph_pointer] = leaf;
//}

PEGraph *ART::convertToPEGraph(vector<Edge *> &v) {
    PEGraph* peGraph = new PEGraph;
    //todo

}

vector<Edge *> ART::convertToVector(PEGraph *pegraph) {
    //todo

}

void ART::edgeSort(vector<vector<Edge *>> &edges) {
    unordered_map<Edge, int> sortBase;
    for (auto &graph : edges) {
        for (auto &edge : graph) {
            if (sortBase.find(*edge) == sortBase.end()) {
                sortBase[*edge] = 1;
            } else {
                sortBase[*edge]++;
            }
        }
    }

    for (auto &graph : edges) {
        sort(graph.begin(), graph.end(), [&sortBase](const Edge *lhs, const Edge *rhs) -> bool {
            return sortBase[*lhs] > sortBase[*rhs];
        });
    }
}

//ART::ART(){}
