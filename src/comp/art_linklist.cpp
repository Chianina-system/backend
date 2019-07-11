//
// Created by guihang on 2019/6/9.
//
#include "art_linklist.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

ART_LinkList::ART_LinkList(){
	//TODO
}

ART_LinkList::~ART_LinkList() {
	//TODO

}

void ART_LinkList::loadGraphStore(const string &file_singleton) {

}

string ART_LinkList::toString() {
    return GraphStore::toString();
}

void ART_LinkList::print(std::ostream &str) {

}

void ART_LinkList::toString_sub(std::ostringstream &strm) {

}

Node *ART_LinkList::insert(vector<Edge *> &v) {
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
    parent->leafNum++;
    return parent;
}

void ART_LinkList::insert(vector<Edge *> v, Node *root, int begin) {
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

vector<Edge *> ART_LinkList::retrieveFromLeaf(Node *node) const {
    vector<Edge *> v;
    while (node->data != nullptr) {
        v.push_back(node->data);
        node = node->parent;
    }
    return v;
}

void ART_LinkList::del(Node *leaf) {
    if (leaf == nullptr)
        return;
    Node *node = leaf->parent;

    // 要删除的图只有一个叶子结点并且叶子结点没有孩子
    if (leaf->leafNum == 1 && leaf->children == nullptr) {

        if (leaf->parent->children->equal(leaf)) {                  // 如果待删除的是头结点
            if (leaf->next) {                                           //如果有后继
                leaf->parent->children = leaf->next;
            } else {                                                    // 如果没有后继
                leaf->parent->children = nullptr;
            }

            delete leaf;
        } else {                                                    // 如果待删除的不是头结点
            Node *before = leaf->parent->children;                      //找到待删除的之前的结点
            while (!before->next->equal(leaf)) {
                before = before->next;
            }
            before->next = leaf->next;
            delete leaf;
        }
    } else {                                                        // 如果leaf大于1 或者Leaf还有孩子，LeafNum - 1
        leaf->leafNum--;
        return;
    }

    while (node->parent && node->leafNum == 0 && node->children == nullptr) {
        Node *temp = node;
        if (node->parent->children->equal(node)) {
            if (node->next) {
                node->parent->children = node->next;
            } else {
                node->parent->children = nullptr;
            }
            delete node;
        } else {
            Node *before = node->parent->children;
            while (!before->next->equal(node)) {
                before = before->next;
            }
            before->next = node->next;
            delete node;
        }
        node = temp->parent;
    }
}

// 卧槽,我忘记当时怎么实现的了
void ART_LinkList::DFS(Node *node) {

    if (node->children) {
        DFS(node->children);
    }
    if (node->next) {
        DFS(node->next);
    }

}

Node *ART_LinkList::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    Node *children = parent->children;
    while (children) {
        if (children->equal(child)) return children;
        children = children->next;
    }
    return nullptr;
}

PEGraph * ART_LinkList::convertToPEGraph(vector<Edge *> &v) const {
    PEGraph* peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;

    for(auto & edge : v){
        vertexid_t src = edge->src;
        vertexid_t dst = edge->des;
        label_t label = edge->label;
        if(graph.find(src) == graph.end()){
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(edge->des, edge->label);

    }

    // todo should we sort the graph?

    peGraph->setGraph(graph);
    return peGraph;
}

vector<Edge *> ART_LinkList::convertToVector(PEGraph *peGraph) {
    vector<Edge *> edgeVector;
    for(auto & it : peGraph->getGraph()){
//        Edge * edge = new Edge()
        int size = it.second.getSize();
        vertexid_t* edges = it.second.getEdges();
        label_t* labels = it.second.getLabels();
        Edge* edge;
        for (int i = 0; i < size; ++i) {
            edge = new Edge(it.first, edges[i], labels[i]);
            edgeVector.push_back(edge);
//            delete edge;      ##这里应该不能delete
        }
    }
    return edgeVector;
}

void ART_LinkList::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {

}
void ART_LinkList::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    Node *node = mapToLeafNode[graph_pointer];
    del(node);
    vector<Edge *> v = convertToVector(pegraph);
    // todo we can sort v before insert
    Node *leaf = insert(v);
    mapToLeafNode[graph_pointer] = leaf;
}

PEGraph * ART_LinkList:: retrieve_locked(PEGraph_Pointer graph_pointer) {

}

PEGraph * ART_LinkList::retrieve(PEGraph_Pointer graph_pointer) {
    if (mapToLeafNode.find(graph_pointer)!= mapToLeafNode.end()){
        Node* node = mapToLeafNode[graph_pointer];
        vector<Edge *> v = retrieveFromLeaf(node);
        return convertToPEGraph(v);
    }
    return nullptr;
}

void ART_LinkList::edgeSort(vector<vector<Edge *>> &edges) {
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

void ART_LinkList::addOneGraph(PEGraph_Pointer pointer, PEGraph *graph) {

}

void ART_LinkList::update_graphs(GraphStore *another) {
    ART_LinkList* another_graphstore = dynamic_cast<ART_LinkList*>(another);
    for(auto& it: another_graphstore->mapToLeafNode){
        update(it.first, retrieve(it.first));               // 这里有不小的问题, 这样实现的话我们的优化实现就完全没有用到了
    }
}

void ART_LinkList::clearEntryOnly() {

}

void ART_LinkList::clear() {
    DFS(root);
}





//ART_LinkList::ART_LinkList(){}
