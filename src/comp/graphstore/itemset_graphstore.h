/*
 * itemset_graphstore.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_

#include "graphstore.h"
#include "edge.h"
#include "itemset_graph.h"

using namespace std;

class ItemsetGraphStore : public GraphStore {


public:

	ItemsetGraphStore(){}

    ~ItemsetGraphStore(){
    	//delete ItemsetGraph in graphs
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		delete it->second;
    	}

    	//delete ItemsetGraph in intToItemset
    	for(auto it = intToItemset.begin(); it != intToItemset.end(); ++it){
    		delete *it;
    	}
    }


    PEGraph* retrieve(PEGraph_Pointer graph_pointer) {
//		//for debugging
//		Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

		PEGraph *out;

		if (graphs.find(graph_pointer) != graphs.end()) {
//			out = new PEGraph(graphs[graph_pointer]);

			//for debugging
			cout << *graphs[graph_pointer] << endl;
			out = convertToPEGraph(graphs[graph_pointer]);
			cout << *out << endl;
		}
		else {
			out = nullptr;
		}

//		//for debugging
//		Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

		return out;
    }

    bool isItemset(int id_edge){
    	return id_edge < 0;
    }


    PEGraph* convertToPEGraph(ItemsetGraph* graph){
    	//collect all the edges
    	vector<int> edges;
    	vector<ItemsetGraph*> itemset_graphs;
    	itemset_graphs.push_back(graph);
    	while(!itemset_graphs.empty()){
    		ItemsetGraph* g = itemset_graphs.back();
    		itemset_graphs.pop_back();
			for(int i = 0; i < g->getLength(); ++i){
				int id_edge = g->getEdgeId(i);
				if(isItemset(id_edge)){
					itemset_graphs.push_back(intToItemset[0 - id_edge - 1]);
				}
				else{
					edges.push_back(id_edge);
				}
			}
    	}
//    	cout << edges.size() << endl;
    	cout << "size of intToEdge: " << intToEdge.size() << endl;

    	//construct pegraph
    	PEGraph* peg = new PEGraph();
   		auto m = peg->getGraph();
    	for(auto it = edges.begin(); it != edges.end(); ++it){
    		int edge_id = *it;
    		Edge edge = intToEdge[edge_id];

    		//for debugging
    		cout << edge << endl;

    		if(m.find(edge.getSrcId()) == m.end()){
    			m[edge.getSrcId()] = EdgeArray();
    		}
  			m[edge.getSrcId()].addOneEdge(edge.getDstId(), edge.getLabel());
    	}

    	cout << *peg << endl;
    	return peg;
    }

    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph(pegraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void loadGraphStore(const string& file, const string& file_in) {
    	//graphstore file
    	this->deserialize(file);

    	this->load_onebyone(file_in);
    }

    void deserialize(const string& file){
    	if(readable){
//    		load_itemsetGraphstore(file);
    		load_onebyone(file);
    	}
    	else{

    	}
    }

    void load_itemsetGraphstore(const string& file){

    }


    void load_onebyone(const string& file){
    	//for debugging
    	Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);

	    std::ifstream fin;
	    fin.open(file);
	    if(!fin) {
	        cout << "can't load file_graphs: " << file << endl;
	    }
	    else{
			std::string line;
			while (getline(fin, line)) {
				if(line == ""){
					continue;
				}

				std::stringstream stream(line);
				std::string id;
				stream >> id;
				PEGraph_Pointer graph_pointer = atoi(id.c_str());
				PEGraph* pegraph = new PEGraph();
				pegraph->load_readable(stream);
				//since the file is appended, we just use the recent updated pegraph
				ItemsetGraph *graph = convertToSetGraph(pegraph);
				if (graphs.find(graph_pointer) != graphs.end()) {
					delete graphs[graph_pointer];
				}
				graphs[graph_pointer] = graph;
			}
			fin.close();

	    	//delete the old graphstore file
	    	FileUtil::delete_file(file);
	    }

		//for debugging
		Logger::print_thread_info_locked("load-readable finished.\n", LEVEL_LOG_FUNCTION);

    }

    ItemsetGraph* convertToSetGraph(PEGraph* pegraph){
    	vector<int> edges_vector;
    	unordered_map<vertexid_t, EdgeArray> g = pegraph->getGraph();
    	for(auto& it : g){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			if(edgeToInt.find(edge) != edgeToInt.end()){
    				edges_vector.push_back(edgeToInt[edge]);
    			}
    			else{
    				intToEdge.push_back(edge);
    				int id = intToEdge.size() - 1;
    				edges_vector.push_back(id);
    				edgeToInt[edge] = id;
    			}
    		}
		}

    	//TODO: replace edge set with frequent itemset
    	compressEdges(edges_vector);
    	ItemsetGraph *graph = new ItemsetGraph(edges_vector);
    	return graph;
    }

    void compressEdges(vector<int>& edges_vector){
    	//TODO

    }

    //shallow copy
    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph* graph) {
    	cout << "shouldn't call this function!" << endl;
    	exit (EXIT_FAILURE);
    }

    static void update_parallel(ItemsetGraphStore* current, NaiveGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
    		assert(current->graphs.find(id) != current->graphs.end());
    		current->update(id, another->getMap().at(id));
    	}
    }

    void update_graphs_parallel(GraphStore* another){
	    //initiate concurrent worklist
	    Concurrent_Worklist<vertexid_t>* worklist = new Concurrent_Workset<vertexid_t>();
	    NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
	    for(auto& it: another_graphstore->getMap()){
	        worklist->push_atomic(it.first);

			//initialize the graphstore
	        if(graphs.find(it.first) == graphs.end()){
	        	graphs[it.first] = new ItemsetGraph();
	        }
	    }

		std::vector<std::thread> comp_threads;
		for (unsigned int i = 0; i < NUM_THREADS; i++)
			comp_threads.push_back(std::thread([=] {update_parallel(this, another_graphstore, worklist);}));

		for (auto &t : comp_threads)
			t.join();

	    //clean
	    delete(worklist);
    }

    void update_graphs_sequential(GraphStore* another){
    	NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
    	for(auto& it: another_graphstore->getMap()){
    		update(it.first, it.second);
    	}
    }

//    void clearEntryOnly() {
//
//    }

//    void clear() {
//    	for(auto it = graphs.begin(); it != graphs.end(); ){
//    		delete it->second;
//    		it = graphs.erase(it);
//    	}
//    }

protected:
    void print(std::ostream& str) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	str << "The number of graphs is: " << graphs.size() << "\n";
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

    void toString_sub(std::ostringstream& str) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	str << "The number of graphs is: " << graphs.size() << "\n";
    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

private:
	//private members
	vector<Edge> intToEdge;
	unordered_map<Edge, int> edgeToInt;

	unordered_map<PEGraph_Pointer, ItemsetGraph*> graphs;

	//a vector storing all the ItemsetGraph
	vector<ItemsetGraph*> intToItemset;


};



#endif /* COMP_GRAPHSTORE_ITEMSET_GRAPHSTORE_H_ */
