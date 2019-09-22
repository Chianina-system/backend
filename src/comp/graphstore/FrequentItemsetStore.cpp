////
//// Created by w on 19-6-30.
////
//
//#include "FrequentItemsetStore.h"
//
//
////全局变量初始化
//std::unordered_map<int, unordered_set<int>> FrequentItemsetStore::intToIdFrequentItemset;
//std::unordered_map<int, Edge> FrequentItemsetStore::idToEdge;
//std::unordered_map<Edge, int> FrequentItemsetStore::edgeToId;
//int FrequentItemsetStore::num_itemset = 0;
//int FrequentItemsetStore::num_edge = 0;
//
//void FrequentItemsetStore::loadGraphStore(const string &file, const string &file_in) {
//    this->deserialize(file);
//
//    this->load_onebyone(file_in);
//}
//
//
//FrequentItemsetStore::~FrequentItemsetStore() {
//
//}
//
//PEGraph *FrequentItemsetStore::retrieve_locked(PEGraph_Pointer graph_pointer) {
//    std::lock_guard<std::mutex> lockGuard(mutex);
//    return retrieve(graph_pointer);
//}
//
//PEGraph *FrequentItemsetStore::retrieve(PEGraph_Pointer graph_pointer) {
////    retrieve the pegraph set
//    std::unordered_set<int> edgeSet = pointerToEdgeIdSet[graph_pointer];
////    std::unordered_set<int> edgeSet = intToEdgeIdSet[graph_pointer];
//
//    if (edgeSet.empty())
//        return nullptr;
//
////    convert it to PEGraph*        ##转换的时间具有时间代价
//    PEGraph *peGraph = convertToPeGraph(edgeSet);
//
//
//    return peGraph;
//}
//
//void FrequentItemsetStore::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//
//    std::unordered_set<int> set_edge = convertToEdgeSet(pegraph);
//    replaceWithAllItemset(set_edge);
//    pointerToEdgeIdSet[graph_pointer] = set_edge;
////    intToEdgeIdSet[graph_pointer] = set_edge;
//    //
//}
//
//void FrequentItemsetStore::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    std::lock_guard<std::mutex> lockGuard(mutex);
//    update(graph_pointer, pegraph);
//
//
//}
//
//// convert from edgeSet to peGraph*
//PEGraph *FrequentItemsetStore::convertToPeGraph(unordered_set<int> &edgeSet) {
//    PEGraph *peGraph = new PEGraph;
//    std::unordered_map<vertexid_t, EdgeArray> graph;
//    unordered_set<int> realSet_Edge;
//
////    getRealSet_Edge(edgeSet, realSet_Edge);
//    realSet_Edge = edgeSet;
//
//    for (auto _edgeId : realSet_Edge) {
//        vertexid_t src = idToEdge[_edgeId].src;
//        vertexid_t dst = idToEdge[_edgeId].des;
//        label_t label = idToEdge[_edgeId].label;
//        const auto &not_found = graph.end();
//        if (graph.find(src) == not_found) {
//            graph[src] = EdgeArray();
//        }
//        graph[src].addOneEdge(dst, label);
//    }
//    peGraph->setGraph(graph);
//    return peGraph;
//}
//
//unordered_set<int> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
//    unordered_set<int> edgeSet;
//    unordered_map<vertexid_t, EdgeArray> &graph = peGraph->getGraph();
//    for (auto &it : graph) {
//
//        int size = it.second.getSize();
//        vertexid_t *edges = it.second.getEdges();
//        label_t *labels = it.second.getLabels();
//        Edge edge;
//        for (int i = 0; i < size; ++i) {
//            edge = Edge(it.first,  labels[i], edges[i]);
//            const auto &not_found = edgeToId.end();
//            if (edgeToId.find(edge) == not_found) {
//                edgeToId[edge] = num_edge;            //edge从哪里开始增加
//                idToEdge[num_edge++] = edge;
//            }
//            edgeSet.insert(edgeToId[edge]);
//        }
//    }
////    change(edgeSet,edgeSet);          //todo change the problem here
//    delete peGraph;
//    return edgeSet;
//}
//
//void FrequentItemsetStore::getRealSet_Edge(unordered_set<int> &graphSet, unordered_set<int> &realEdgeSet) {
//
//    for (int it : graphSet) {
//        if (it >= 0) realEdgeSet.insert(it);
//        else getRealSet_Edge(intToIdFrequentItemset[it], realEdgeSet);
////        else getRealSet_Edge(intToEdgeIdSet[it], realEdgeSet);
//    }
//}
//
//std::vector<std::unordered_set<int>>
//FrequentItemsetStore::frequentItemsetMining_closed(vector<unordered_set<int>> &graphs) {
//    writeToFile(graphs);
//
//    system("../lib/eclat -tc ../lib/file/intSets.txt ../lib/file/result.txt");            //todo 理解这里系数的含义
//
//    std::vector<std::unordered_set<int>> v = readFromFile();            // only read the first 10 line
//    return v;
//}
//
//std::vector<std::unordered_set<int>>
//FrequentItemsetStore::frequentItemsetMining_minimum(vector<unordered_set<int>> &graphs) {
//    writeToFile(graphs);
//
//    system("../lib/eclat -tm ../lib/file/intSets.txt ../lib/file/result.txt");            // should we stop for a while?
//
//    std::vector<std::unordered_set<int>> v = readFromFile();            // only read the first line
//    return v;
//}
//
//
//FrequentItemsetStore::FrequentItemsetStore() = default;
//
///// 使用itemset替换掉 edgeset 中的元素
///// \param set_edge
///// \param itemset[gr
//void FrequentItemsetStore::replaceWithItemset(unordered_set<int> &set_edge, unordered_set<int> &itemset) {
//    //删除 itemset中的元素
//    for (auto &item : itemset) {
//        set_edge.erase(item);
//    }
//    num_itemset++;
//    int newEdge = -num_itemset;
//    set_edge.insert(newEdge);
//    intToIdFrequentItemset[newEdge] = itemset;
//}
//
////FrequentItemsetStore::FrequentItemsetStore(vector<unordered_set<int>> graphs) {         //没有用到该构造函数
////    std::vector<std::unordered_set<int>> vector_itemset = frequentItemsetMining_closed(graphs);
////    //compute the frequent itemset
////    for (auto &graph: graphs) {
////        bool flag = true;           //check if the graphSet need to be updated
////        auto const not_found = graph.end();
////        for (auto &itemset : vector_itemset) {
//////            graph.count(elem)
////            for (auto &item : itemset) {
////                if (graph.find(item) == not_found) {          //如果发现有itemset中的元素不存在于graph,不需要进行更新
////                    flag = false;
////                    break;
////                }
////            }
////            if (flag) {               //如果需要更新,则进行更新操作
////                replaceWithItemset(graph, itemset);
////            }
////        }
////    }
////}
//
/////
///// \param graphs 将文件写入 filePath 中去
//void FrequentItemsetStore::writeToFile(vector<unordered_set<int>> &graphs) {
//    ofstream output;
//    output.open(inputFilePath);
//
//    for (const auto &graph : graphs) {
//        for (auto _edgeId: graph) {
//            cout << _edgeId << " ";
//        }
//        cout << endl;
//    }
//    output.close();
//}
//
////从挖掘结果中选取频繁项集挖掘的结果,目前的策略是简单选取前10行
//vector<unordered_set<int>> FrequentItemsetStore::readFromFile() {
//    vector<unordered_set<int>> vector_itemSet;
//
//    ifstream finput;
//    finput.open(resultPath);
//    if (!finput) {
//        cout << "can't load file: " << resultPath << endl;
//        exit(EXIT_FAILURE);
//    }
//
//    std::string line;
//    int cnt = 0;
//    int num = 10;
//    while (getline(finput, line) && !line.empty() && ++cnt <= num) {     //读取前10行
//        unordered_set<int> itemset;
//        std::stringstream stream(line);
//        int id;
//        while (stream >> id) itemset.insert(id);
//        vector_itemSet.push_back(itemset);
//    }
//    finput.close();
//    return vector_itemSet;
//}
//
//void FrequentItemsetStore::addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) {
//    std::lock_guard<std::mutex> lockGuard(mutex);
//    update(pointer, graph);
//
////    auto set = convertToEdgeSet(graph);
////    another_update(pointer, set);
//}
//
//void FrequentItemsetStore::update_graphs(GraphStore *another) {
//    update_graphs_sequential(another); // sequential
////    update_graphs_parallel(another); // in parallel
//}
//
//void FrequentItemsetStore::clearEntryOnly() {
//
//}
//
//void FrequentItemsetStore::clear() {
//    // run_inmemory中无需进行内存处理
//    //todo 在run_ooc中,对一个partition处理结束后,应该将属于该patition全局变量初始化
//}
//
//
//void FrequentItemsetStore::print(std::ostream &str) {
//    // todo
//}
//
//void FrequentItemsetStore::toString_sub(std::ostringstream &strm) {
//    //todo
//}
//
//void FrequentItemsetStore::deserialize(const string &file) {
//    //考虑先保证正确性的目的,原先的实现上复用了naivegraph中的readable与unreadable方法,但是unreadable方法存在一些问题.
//    //todo
//}
//
//void FrequentItemsetStore::serialize(const string &file) {
//    //考虑先保证正确性的目的,原先的实现上复用了naivegraph中的readable与unreadable方法,但是unreadable方法存在一些问题.
//    //todo
//}
//
//void FrequentItemsetStore::load_onebyone(const string &file) {
//    //考虑先保证正确性的目的,原先的实现上复用了naivegraph中的readable与unreadable方法,但是unreadable方法存在一些问题.
//    //todo
//}
//
//void FrequentItemsetStore::update_graphs_sequential(GraphStore *another) {
//    FrequentItemsetStore *another_graphstore = dynamic_cast<FrequentItemsetStore *>(another);
//    for (auto &it: another_graphstore->pointerToEdgeIdSet) {
////        PEGraph *peGraph = convertToPeGraph(it.second);
////        update(it.first, peGraph);              //这里的两个系数应该没有错吧
//        another_update(it.first, it.second);
//    }
//}
//
//void FrequentItemsetStore::update_graphs_parallel(GraphStore *another) {
//    //
//    // todo 没有实现
//}
//
///// 使用频繁项集对表示图的set进行元素替换
///// \param set_edge 一个图中所有edge所对应的id
//void FrequentItemsetStore::replaceWithAllItemset(unordered_set<int> &set_edge) {
//    for (auto pair: intToIdFrequentItemset) {
//        auto &idFrequentItemset = pair.second;
//        auto not_found = set_edge.end();
//        bool need_update = true;
//        for (auto id: idFrequentItemset) {
//            if (set_edge.find(id) == not_found) {
//                need_update = false;
//                break;
//            }
//        }
//        if (need_update) replaceWithItemset(set_edge, idFrequentItemset);
//    }
//}
//
//void FrequentItemsetStore::another_update(const PEGraph_Pointer first, unordered_set<int> &set) {
//    pointerToEdgeIdSet[first] = set;
//}
//
///// 对于run_inmemory,由于开始时图为空,无法进行频繁项集挖掘,调用该函数在计算结束后测试频繁项集挖掘是否正常
//void FrequentItemsetStore::testFrequentItemsetStore() {
//    vector<unordered_set<int> > graphs;
//    for(auto & it : pointerToEdgeIdSet){
//        graphs.push_back(it.second);
//    }
//
//    //计算并返回频繁项集
//    std::vector<std::unordered_set<int>> vector_itemset = frequentItemsetMining_closed(graphs);     //对graphs进行频繁项集挖掘
//    for (auto &graph: graphs) {
//        bool flag = true;           //检查是否需要进行更新,如果一个edgeset含有某一个频繁项集的全部元素,那么进行更新操作
//        auto const not_found = graph.end();
//        for (auto &itemset : vector_itemset) {
////            graph.count(elem)
//            for (auto &item : itemset) {
//                if (graph.find(item) == not_found) {          //如果发现有itemset中的元素不存在于graph,不需要进行更新
//                    flag = false;
//                    break;
//                }
//            }
//            if (flag) {               //如果需要更新,则进行更新操作
//                replaceWithItemset(graph, itemset);
//            }
//        }
//    }
//
//}
//
//
//
//
//
