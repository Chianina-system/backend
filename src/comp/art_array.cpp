//
// Created by w on 19-7-11.
//

#include <stack>
#include "art_array.h"

PEGraph *ART_array::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph *ART_array::retrieve(PEGraph_Pointer graph_pointer) {
    if (mapToLeafNode.find(graph_pointer) != mapToLeafNode.end()) {
        Node *node = mapToLeafNode[graph_pointer];
        vector<Edge *> v = retrieveFromLeaf(node);
        return convertToPEGraph(v);
    }
    return nullptr;
}

void ART_array::update_locked(PEGraph_Pointer graph_pointer, PEGraph *peGraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, peGraph);
}

void ART_array::update(PEGraph_Pointer graph_pointer, PEGraph *peGraph) {
    Node *node = mapToLeafNode[graph_pointer];
    del(node);
    vector<Edge *> v = convertToVector(peGraph);
    // todo we can sort v before insert
    Node *leaf = insert(v);
    mapToLeafNode[graph_pointer] = leaf;
}

void ART_array::loadGraphStore(const string &file) {

}

void ART_array::addOneGraph(PEGraph_Pointer pointer, PEGraph *peGraph) {
    vector<Edge *> edges = convertToVector(peGraph);     //##这里是不是返回引用更好一些
    // 对vector进行排序
    sort(edges.begin(), edges.end(), [=](const Edge *lhs, const Edge *rhs) -> bool {
        return sortBase[*lhs] > sortBase[*rhs];
    });

    Node *leaf = insert(edges);
    mapToLeafNode[pointer] = leaf;
    mapToLeafNum[leaf]++;
}


void ART_array::update_graphs(GraphStore *another) {
    ART_array *another_graphstore = dynamic_cast<ART_array *>(another);
    for (auto &it: another_graphstore->mapToLeafNode) {
        update(it.first, retrieve(it.first));               // 这里有不小的问题, 这样实现的话我们的优化实现就完全没有用到了
    }
}

void ART_array::clearEntryOnly() {

}

void ART_array::clear() {
    postOrderDelete_iteration(root);
}

string ART_array::toString() {
    return GraphStore::toString();
}

void ART_array::print(std::ostream &str) {

}

void ART_array::toString_sub(std::ostringstream &strm) {

}


ART_array::~ART_array() {
    clear();
}

void ART_array::del(Node *leaf) {
    //  如果leaf是root
    if (leaf->data == nullptr) return;
    // 如果有多个相同的图
    if (mapToLeafNum[leaf] > 1) {
        mapToLeafNum[leaf]--;
        return;
    }
    // 如果leaf还有孩子
    if (mapToLeafNum[leaf] == 1 && !leaf->children.empty()) {
        mapToLeafNum[leaf]--;
        return;
    }
    //如果leaf没有孩子
    mapToLeafNum[leaf]--;
    Node *parent = leaf->parent;
    while (leaf->data && leaf->children.empty()) {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), leaf), parent->children.end());
        delete leaf;
        leaf = parent;
    }
}

vector<Edge *> ART_array::convertToVector(PEGraph *peGraph) {
    vector<Edge *> edgeVector;
    for (auto &it : peGraph->getGraph()) {
//        Edge * edge = new Edge()
        int size = it.second.getSize();
        vertexid_t *edges = it.second.getEdges();
        label_t *labels = it.second.getLabels();
        Edge *edge;
        for (int i = 0; i < size; ++i) {
            edge = new Edge(it.first, edges[i], labels[i]);
            edgeVector.push_back(edge);
//            delete edge;      ##这里应该不能delete
        }
    }
    return edgeVector;
}

Node *ART_array::insert(vector<Edge *> &v) {
    if (v.empty()) return nullptr;
    Node *parent = root;
    for (auto &i : v) {
        Node *child = findChild(parent, i);
        if (!child) {
            child = new Node();
            child->data = new Edge(*i);
            child->parent = parent;
            parent->insert_binarySearch(child);
        }
        parent = child;
    }
    mapToLeafNum[parent]++;
    return parent;
}

//由于使用vector因此我们可以使用二分查找
Node *ART_array::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    vector<Node *> children = parent->children;
    //TODO 二分查找
    int left = 0, right = children.size() - 1;
    int mid;
    while (left <= right) {
        mid = (left + right) / 2;
        if (children[mid]->data == child)
            return children[mid];
        else if (children[mid]->data > child)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return nullptr;
}

void ART_array::postOrderDelete_recursion(Node *root) {
    for (auto child: root->children) {
        postOrderDelete_recursion(child);
    }
    //析构掉一个node
    delete root;
}

vector<Edge *> ART_array::retrieveFromLeaf(Node *node) {
    vector<Edge *> v;
    while (node->data != nullptr) {
        v.push_back(node->data);
        node = node->parent;
    }
    return v;
}

PEGraph *ART_array::convertToPEGraph(vector<Edge *> &v) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;

    for (auto &edge : v) {
        vertexid_t src = edge->src;
        vertexid_t dst = edge->des;
        label_t label = edge->label;
        if (graph.find(src) == graph.end()) {
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }

    // todo should we sort the graph?

    peGraph->setGraph(graph);
    return peGraph;
}

void ART_array::postOrderDelete_iteration(Node *root) {
    if (root == nullptr) return;

    std::stack<Node *> stk;
    stk.push(root);
    while (!stk.empty()) {
        Node *top = stk.top();
        stk.pop();
        for (auto child:top->children) {
            stk.push(child);
        }
        // delete the node
        delete top;
    }
}

void ART_array::edgeSort(vector<vector<Edge *>> &graphs) {

    for (auto &graph : graphs) {
        for (auto &edge : graph) {
            if (sortBase.find(*edge) == sortBase.end()) {
                sortBase[*edge] = 1;
            } else {
                sortBase[*edge]++;
            }
        }
    }

    for (auto &graph : graphs) {
        sort(graph.begin(), graph.end(), [=](const Edge *lhs, const Edge *rhs) -> bool {
            return sortBase[*lhs] > sortBase[*rhs];
        });
    }
}

