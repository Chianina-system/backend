//
// Created by w on 19-7-11.
//

#include <stack>
#include "art_array.h"

PEGraph *art_array::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph *art_array::retrieve(PEGraph_Pointer graph_pointer) {
    if (mapToLeafNode.find(graph_pointer)!= mapToLeafNode.end()){
        Node* node = mapToLeafNode[graph_pointer];
        vector<Edge *> v = retrieveFromLeaf(node);
        return convertToPEGraph(v);
    }
    return nullptr;
}

void art_array::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, pegraph);
}

void art_array::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    Node *node = mapToLeafNode[graph_pointer];
    del(node);
    vector<Edge *> v = convertToVector(pegraph);
    // todo we can sort v before insert
    Node *leaf = insert(v);
    mapToLeafNode[graph_pointer] = leaf;
}

void art_array::loadGraphStore(const string &file) {

}

void art_array::addOneGraph(PEGraph_Pointer pointer, PEGraph *graph) {
    vector<Edge *> vector = convertToVector(graph);     //##这里是不是返回引用更好一些
    Node *leaf = insert(vector);
    mapToLeafNode[pointer]=leaf;
    mapToLeafNum[leaf]++;
}

void art_array::update_graphs(GraphStore *another) {
    art_array* another_graphstore = dynamic_cast<art_array*>(another);
    for(auto& it: another_graphstore->mapToLeafNode){
        update(it.first, retrieve(it.first));               // 这里有不小的问题, 这样实现的话我们的优化实现就完全没有用到了
    }
}

void art_array::clearEntryOnly() {

}

void art_array::clear() {
    postOrderDelete_iteration(root);
}

string art_array::toString() {
    return GraphStore::toString();
}

void art_array::print(std::ostream &str) {

}

void art_array::toString_sub(std::ostringstream &strm) {

}


art_array::~art_array() {
    clear();
}

void art_array::del(Node *leaf) {
    //  如果leaf是root
    if(leaf->data == nullptr) return;
    // 如果有多个相同的图
    if(mapToLeafNum[leaf]>1) {
        mapToLeafNum[leaf]--;
        return;
    }
    // 如果leaf还有孩子
    if(mapToLeafNum[leaf]==1 && !leaf->children.empty()){
        mapToLeafNum[leaf]--;
        return;
    }
    //如果leaf没有孩子
    mapToLeafNum[leaf]--;
    Node* parent = leaf->parent;
    while (leaf->data && leaf->children.empty()) {
        parent->children.erase(remove(parent->children.begin(),parent->children.end(),leaf),parent->children.end());
        delete leaf;
        leaf = parent;
    }
}

vector<Edge *> art_array::convertToVector(PEGraph *peGraph) {
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

Node * art_array::insert(vector<Edge *> &v) {
    if (v.empty()) return nullptr;
    Node *parent = root;
    for (auto &i : v) {
        Node *child = findChild(parent, i);
        if (!child) {
            child = new Node();
            child->data = new Edge(*i);
            child->parent = parent;
            // TODO 通过二分查找的方式插入child, 那么有必要修改findChild函数,或者重载一个
            parent->children.push_back(child);
        }
        parent = child;
    }
//    parent->leafNum++;
    mapToLeafNum[parent]++;         // ##是否需要初始化
    return parent;
}

//由于使用vector因此我们可以使用二分查找
Node *art_array::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    vector<Node *> children = parent->children;
    //TODO 二分查找
//    while (children) {
//        if (children->equal(child)) return children;
//        children = children->next;
//    }
    return nullptr;
}

void art_array::postOrderDelete_recursion(Node *root) {
    for(auto child: root->children){
        postOrderDelete_recursion(child);
    }
    //析构掉一个node
    delete root;
}

vector<Edge *> art_array::retrieveFromLeaf(Node *node) {
    vector<Edge*> v;
    while (node->data != nullptr){
        v.push_back(node->data);
        node = node->parent;
    }
    return v;
}

PEGraph *art_array::convertToPEGraph(vector<Edge *> &v) {
    PEGraph* peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;

    for(auto & edge : v){
        vertexid_t src = edge->src;
        vertexid_t dst = edge->des;
        label_t label = edge->label;
        if(graph.find(src) == graph.end()){
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }

    // todo should we sort the graph?

    peGraph->setGraph(graph);
    return peGraph;
}

void art_array::postOrderDelete_iteration(Node *root) {
    if(root == nullptr) return;

    std::stack<Node*> stk;
    stk.push(root);
    while (!stk.empty()){
        Node* top = stk.top();
        stk.pop();
        for(auto child:top->children){
            stk.push(child);
        }
        // delete the node
        delete top;
    }
}
