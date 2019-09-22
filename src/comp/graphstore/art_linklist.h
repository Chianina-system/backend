///*
// * art.h
// *
// *  Created on: May 22, 2019
// *      Author: zqzuo
// */
//
//#ifndef COMP_ART_H_
//#define COMP_ART_H_
//
//#include "graphstore.h"
//
//using namespace std;
//
//struct Edge {
//    int src;
//    int des;
//    char label;
//
//    Edge() {};
//
//    Edge(int src, char label, int des) : src(src), label(label), des(des) {};
//
//    Edge(const Edge &edge) {
//        this->src = edge.src;
//        this->des = edge.des;
//        this->label = edge.label;
//    }
//
//    bool equal(Edge *other) {
//        if (!other) return false;
//        return this->src == other->src && this->des == other->des && this->label == other->label;
//    }
//
//    bool operator==(const Edge &rhs) const {
//        return src == rhs.src &&
//               des == rhs.des &&
//               label == rhs.label;
//    }
//
//    bool operator<(const Edge &rhs) const {
//        if (src < rhs.src)
//            return true;
//        if (rhs.src < src)
//            return false;
//        if (des < rhs.des)
//            return true;
//        if (rhs.des < des)
//            return false;
//        return label < rhs.label;
//    }
//
//
//
//    bool operator!=(const Edge &rhs) const {
//        return !(rhs == *this);
//    }
//
//    friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
//        os << "src: " << edge.src << " des: " << edge.des << " label: " << edge.label;
//        return os;
//    }
//};
//
//// declare hash<Edge>
//namespace std {
//    template<>
//    struct hash<Edge> {
//        std::size_t operator()(const Edge &e) const {
//            using std::hash;
//            // Compute individual hash values for first,
//            // second and third and combine them using XOR
//            // and bit shifting:
//            return ((hash<int>()(e.src)
//                     ^ (hash<int>()(e.des) << 1)) >> 1)
//                   ^ (hash<char>()(e.label) << 1);
//        }
//    };
//
//}
//
//
//struct Node {
//    Edge *data;
//    Node *parent;
//    Node *children;
//    Node *next;
//    int leafNum;
//
//    Node() : parent(nullptr), children(nullptr), data(nullptr), next(nullptr), leafNum(0) {};
//
//    ~Node(){
//        delete data;
//    }
//
//    bool equal(Node *other) {
//        if (!other)return false;
//        return this->data->equal(other->data);
//    }
//
//    bool equal(Edge *other) {
//        if (!other)return false;
//        return this->data->equal(other);
//    }
//
//    void toString() {
//        cout << this->data->src << " " << this->data->label << " " << this->data->des << endl;
//    }
//
//};
//
//
//
//
//
//class ART_LinkList : public GraphStore {
//
//public:
//    ART_LinkList();
//
//    ~ART_LinkList();
//
////    void init(CFG* cfg) {
////
////    }
//
//    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;
//
//    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;
//
//    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;
//
//    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;
//
//    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) override;
//
//    void update_graphs(GraphStore *another) override;
//
//    void clearEntryOnly() override;
//
//    void clear() override;
//
//    Node *insert(vector<Edge *> &v);
//
//    void insert(vector<Edge *> v, Node *root, int begin);
//
//    vector<Edge *> retrieveFromLeaf(Node *node) const;
//
//    static void del(Node *leaf);
//
//    void DFS(Node *node, Node *head);
//
//    void edgeSort(vector<vector<Edge *>> &edges);
//
//    Node *findChild(Node *parent, Edge *child);
//
//    PEGraph * convertToPEGraph(vector<Edge *> &v) const;
//
//    vector<Edge *> convertToVector(PEGraph *peGraph);
//
////    void loadGraphStore(const string &file) override;
//    void loadGraphStore(const string& file, const string& file_in);
//
//    string toString() override;
//
//protected:
//    void print(std::ostream &str) override;
//
//    void toString_sub(std::ostringstream &strm) override;
//
////    void addOneSingleton(vertexid_t t);
//
//private:
//    Node *root = new Node();
//    std::unordered_map<PEGraph_Pointer, Node *> mapToLeafNode;
////    std::set<vertexid_t> singletonSet;
//
//    void deleteLinkList(Node *head);
//};
//
//
//#endif /* COMP_ART_H_ */
