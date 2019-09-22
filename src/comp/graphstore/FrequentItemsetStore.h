////
//// Created by w on 19-6-30.
////
//
//#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
//#define BACKEND1_FREQUENTITEMSETSTORE_H
//
//
////#include "CommonLibs.hpp"
//#include "graphstore.h"
//#include "art.h"
//
//const string inputFilePath = "../lib/file/intSets.txt";           // 将图信息存储至该文件中, 供eclat命令调用
//const string resultPath = "../lib/file/result.txt";           // 将图信息存储至该文件中, 供eclat命令调用
//
//class FrequentItemsetStore : public GraphStore {
//
//public:
//
//    FrequentItemsetStore();
//
//    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) override;
//
//    void update_graphs(GraphStore *another) override;
//
//    void clearEntryOnly() override;
//
//    void clear() override;
//
//    void testFrequentItemsetStore();
//
//    void printOutInfo(){
//
//        int size_graphs = pointerToEdgeIdSet.size();
//        long size_edges = 0;
//
//        cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
//
//        for(auto it = pointerToEdgeIdSet.begin(); it != pointerToEdgeIdSet.end(); ++it){
////    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
//            PEGraph *peGraph = convertToPeGraph(it->second);
//            size_edges += peGraph->getNumEdges();
//        }
//
//        cout << "Number of graphs: " << size_graphs << endl;
//        cout << "Number of edges: " << size_edges << endl;
//
//        testFrequentItemsetStore();
//    }
//
//protected:
//    void print(std::ostream &str) override;
//
//    void toString_sub(std::ostringstream &strm) override;
//
//private:
//
//    FrequentItemsetStore(vector<unordered_set<int>> graphs);
//
//    ~FrequentItemsetStore() override;
//
//    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;
//
//    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;
//
//    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;
//
//    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;
//
//    void loadGraphStore(const string& file, const string& folder_in) override;
//
//private:
//
//    std::unordered_map<PEGraph_Pointer, std::unordered_set<int>> pointerToEdgeIdSet;    // 使用到的图的EdgeIdSet实体
//    static std::unordered_map<int, unordered_set<int>> intToIdFrequentItemset;          // 使用到的频繁项集实体
//    static std::unordered_map<int, Edge> idToEdge;                                      // Edge与其对应的id的相互映射, 可以只使用一个, 但在查询上会慢一些
//    static std::unordered_map<Edge, int> edgeToId;
//    static int num_itemset;     //      当前储存的频繁项集数目,同时在计算过程中 -num_itemset 作为intToIdFrequentItemset的Key
//    static int num_edge;        //      当前储存的Edge数目,同时在计算过程中 num_edge 作为 pointerToEdgeIdSet的Key
//
//
//    PEGraph *convertToPeGraph(unordered_set<int> &edgeSet);         //将一个图的EdgeIdSet还原成pegraph,在retrieve中被调用
//
//    unordered_set<int> convertToEdgeSet(PEGraph *peGraph);          //将一个图中的所有边对应的id存到set<int>中
//
//    void getRealSet_Edge(unordered_set<int> &graphSet, unordered_set<int> &realEdgeSet);    //将一个可能包含频繁项集的set(包含负数元素)还原成只包含edgeid的set
//
//    std::vector<std::unordered_set<int>> frequentItemsetMining_closed(vector<unordered_set<int>> &graphs);  //不同的挖掘方式,具体eclat指令的使用方式见README_FI.md
//
//    std::vector<std::unordered_set<int>> frequentItemsetMining_minimum(vector<unordered_set<int>> &graphs);
//
//    void writeToFile(vector<unordered_set<int>> &graphs);       //将store中所有图的set存入文件中,以供调用
//
//    vector<unordered_set<int>> readFromFile();                  // 从文件中读取频繁项集挖掘的结果
//
//
//    void deserialize(const string &file);
//
//    void serialize(const string &file);
//
//    void load_onebyone(const string &file);
//
//    void replaceWithItemset(unordered_set<int> &set_edge, unordered_set<int> &itemset);
//
//    void update_graphs_sequential(GraphStore *another);
//
//    void update_graphs_parallel(GraphStore *another);
//
//    void replaceWithAllItemset(unordered_set<int> &set_edge);
//
//    void another_update(const PEGraph_Pointer first, unordered_set<int> &set);     //更适合这个类的update方法,在这个类中,直接将set作为value重新赋值即可
//
//};
//
//
//#endif //BACKEND1_FREQUENTITEMSETSTORE_H
