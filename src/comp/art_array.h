#include <utility>

//
// Created by w on 19-7-11.
//

#ifndef BACKEND_NJU_ART_ARRAY_H
#define BACKEND_NJU_ART_ARRAY_H


#include "graphstore.h"

struct Edge {
    int src;
    int des;
    char label;

    Edge() {};

    Edge(int src, char label, int des) : src(src), label(label), des(des) {};

    Edge(const Edge &edge) {
        this->src = edge.src;
        this->des = edge.des;
        this->label = edge.label;
    }

    bool equal(Edge *other) {
        if (!other) return false;
        return this->src == other->src && this->des == other->des && this->label == other->label;
    }

    bool operator==(const Edge &rhs) const {
        return src == rhs.src &&
               des == rhs.des &&
               label == rhs.label;
    }

    bool operator<(const Edge &rhs) const {
        if (src < rhs.src)
            return true;
        if (rhs.src < src)
            return false;
        if (des < rhs.des)
            return true;
        if (rhs.des < des)
            return false;
        return label < rhs.label;
    }

    bool operator!=(const Edge &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
        os << "src: " << edge.src << " des: " << edge.des << " label: " << edge.label;
        return os;
    }
};

// declare hash<Edge>
namespace std {
    template<>
    struct hash<Edge> {
        std::size_t operator()(const Edge &e) const {
            using std::hash;
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:
            return ((hash<int>()(e.src)
                     ^ (hash<int>()(e.des) << 1)) >> 1)
                   ^ (hash<char>()(e.label) << 1);
        }
    };
}

struct Node {
    Node();

    ~Node(){
        delete data;
     }

    Edge *data;
    Node *parent;
    vector<Node*> children;

    bool equal(Node *other) {
        if (!other)return false;
        return this->data->equal(other->data);
    }

    bool equal(Edge *other) {
        if (!other)return false;
        return this->data->equal(other);
    }

    void insert_binarySearch(Node *child) {
        this->children.insert(lower_bound(this->children.begin(), this->children.end(), child), child);
    }
};

class ART_array : public GraphStore{
public:
    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    ART_array();

    virtual ~ART_array();

    void init(CFG* cfg) {

    }

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *peGraph) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *peGraph) override;

//    void loadGraphStore(const string &file) override;
    void loadGraphStore(const string& file, const string& file_in);

    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *peGraph) override;

    void update_graphs(GraphStore *another) override;

    void clearEntryOnly() override;

    void clear() override;

    string toString() override;

protected:
    void print(std::ostream &str) override;

    void toString_sub(std::ostringstream &strm) override;

private:
    Node *root;
    unordered_map<PEGraph_Pointer ,Node *> mapToLeafNode;
    unordered_map<Node*, int> mapToLeafNum;
    unordered_map<Edge, int> sortBase;

    void del(Node *leaf);

    vector<Edge *> convertToVector(PEGraph *peGraph);

    Node * insert(vector<Edge *> &v);

    Node *findChild(Node *parent, Edge *child);

    static void postOrderDelete_recursion(Node *root);

    static vector<Edge *> retrieveFromLeaf(Node *node);

    static PEGraph *convertToPEGraph(vector<Edge *> &v);

    static void postOrderDelete_iteration(Node *root);

    void edgeSort(vector<vector<Edge *>> &graphs);
};


#endif //BACKEND_NJU_ART_ARRAY_H
