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

const string inputFile = "../lib/file/input_itemsets.txt";
const string outFile = "../lib/file/out_itemsets.txt";
const bool serialize_peg_mode = true;

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
			out = convertToPEGraph(graphs[graph_pointer]);
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
			for(unsigned int i = 0; i < g->getLength(); ++i){
				int id_edge = g->getEdgeId(i);
				if(isItemset(id_edge)){
					itemset_graphs.push_back(intToItemset[getItemsetIndex(id_edge)]);
				}
				else{
					edges.push_back(id_edge);
				}
			}
    	}
//    	cout << edges.size() << endl;
//    	cout << "size of intToEdge: " << intToEdge.size() << endl;

    	//construct pegraph
    	PEGraph* peg = new PEGraph();
    	for(auto it = edges.begin(); it != edges.end(); ++it){
    		int edge_id = *it;
    		Edge edge = intToEdge[edge_id];

//    		//for debugging
//    		cout << edge << endl;

    		if(peg->getGraph().find(edge.getSrcId()) == peg->getGraph().end()){
    			peg->getGraph()[edge.getSrcId()] = EdgeArray();
    		}
    		peg->getGraph()[edge.getSrcId()].addOneEdge(edge.getDstId(), edge.getLabel());
    	}
    	//sort all the edgeArrays in peg
    	for(auto it = peg->getGraph().begin(); it != peg->getGraph().end(); ++ it){
			(*it).second.sort();
    	}

//    	cout << *peg << endl;
//    	cout << peg->getGraph().size() << endl;
    	return peg;
    }

    void update_convert_locked(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//		//for debugging
//		Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

		if (graphs.find(graph_pointer) != graphs.end()) {
			delete graphs[graph_pointer];
		}

		//for debugging
//		cout << *pegraph << endl;
		graphs[graph_pointer] = convertToSetGraph_locked(pegraph);
//		cout << *graphs[graph_pointer] << endl;
//		cout << *convertToPEGraph(graphs[graph_pointer]) << endl;

//		//for debugging
//		Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
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
    	if(serialize_peg_mode){
			//graphstore file
			this->deserialize(file);

			//load in-graphs
			this->load_onebyone(file_in);

			//construct itemset base
			this->constructItemsetBase();
    	}
    	else{
    		this->deserialize(file);
    		this->load_onebyone(file_in);
    	}

    }

    void serialize(const string& file){
    	if(readable){
    		if(serialize_peg_mode){
				ofstream myfile;
				myfile.open(file, std::ofstream::out);
				if (myfile.is_open()){
					for (auto& n : graphs) {
						//write a pegraph into file
						myfile << n.first << "\t";
						PEGraph* out = convertToPEGraph(n.second);
						out->write_readable(myfile);
						delete out;
						myfile << "\n";
					}
					myfile.close();
				}
    		}
    		else{//direct written in the itemset format
    			store_itemsetGraphstore(file);
    		}
    	}
    	else{

    	}
    }

    void deserialize(const string& file){
    	if(readable){
    		if(serialize_peg_mode){
				load_onebyone(file);
    		}
    		else{
				load_itemsetGraphstore(file);
    		}
    	}
    	else{

    	}
    }

    //load the graphstore in the itemset format
    void load_itemsetGraphstore(const string& file){

    }


    void store_itemsetGraphstore(const string& file){

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
				delete pegraph;
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

//    ItemsetGraph* convertToSetGraph(PEGraph* pegraph){
//    	set<int> edges_set;
//    	for(auto& it : pegraph->getGraph()){
//    		vertexid_t src = it.first;
//    		int size = it.second.getSize();
//    		vertexid_t* dsts = it.second.getEdges();
//    		label_t* labels = it.second.getLabels();
//    		for(int i = 0; i < size; ++i){
//    			Edge edge(src, dsts[i], labels[i]);
//    			addNewEdgeToEdgeset(edge, edges_set);
//    		}
//		}
//
//    	//TODO: replace edge set with frequent itemset
//    	compressEdges(edges_set);
//    	ItemsetGraph *graph = new ItemsetGraph(edges_set);
//    	return graph;
//    }

    ItemsetGraph* convertToSetGraph(PEGraph* pegraph){
    	int len = pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    ItemsetGraph* convertToSetGraph_locked(PEGraph* pegraph){
    	int len = pegraph->getNumEdges();
    	MyArray* myArray = new MyArray(len);

    	for(auto& it : pegraph->getGraph()){
    		vertexid_t src = it.first;
    		int size = it.second.getSize();
    		vertexid_t* dsts = it.second.getEdges();
    		label_t* labels = it.second.getLabels();
    		for(int i = 0; i < size; ++i){
    			Edge edge(src, dsts[i], labels[i]);
    			addNewEdgeToEdgeset_locked(edge, myArray);
    		}
		}

    	//sort the array
    	sort(myArray->getData(), myArray->getData() + myArray->getLength());

    	//TODO: replace edge set with frequent itemset
    	compressEdges(myArray);
    	ItemsetGraph *graph = new ItemsetGraph(myArray);
    	delete myArray;
    	return graph;
    }

    void addNewEdgeToEdgeset_locked(Edge& edge, MyArray* myArray){
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	addNewEdgeToEdgeset(edge, myArray);
    }

    void addNewEdgeToEdgeset(Edge& edge, MyArray* myArray){
		if(edgeToInt.find(edge) != edgeToInt.end()){
			myArray->insert(edgeToInt[edge]);
		}
		else{
	    	intToEdge.push_back(edge);
	    	int id = intToEdge.size() - 1;
	    	edgeToInt[edge] = id;
	    	myArray->insert(id);
		}
    }

    /*
     *encode or compress the graph representation by replacing certain frequent subsets of edge ids with the unique itemset ids
     */
    void compressEdges(MyArray* myArray){
    	for(unsigned i = 0; i < intToItemset.size(); i++){
    		if(myArray->getLength() < intToItemset.at(intToItemset.size() - 1)->getLength()){
    			break;
    		}
    		subsume(myArray, intToItemset[i], getItemsetId(i));
    	}
    }

    int getItemsetId(unsigned index){
    	return 0 - (index + 1);
    }

    unsigned getItemsetIndex(int id_itemset){
    	return 0 - (id_itemset + 1);
    }

    bool subsume(MyArray* myArray, ItemsetGraph* itemset, int id_itemset){
    	//first, determine if the itemset is a subset of edges_set; note that both are sorted
    	unsigned int i = 0, j = 0;
    	while(j < myArray->getLength() && i < itemset->getLength()){
    		if(myArray->getData(j) < itemset->getEdgeId(i)){
    			j++;
    		}
    		else if(myArray->getData(j) == itemset->getEdgeId(i)){
    			j++;
    			i++;
    		}
    		else {//if(myArray->getData(j) > itemset->getEdgeId(i))
    			return false;
    		}
    	}
    	if(i < itemset->getLength()){
    		return false;
    	}

    	//second, if so, replace the itemset in edges_set with the itemset id
    	unsigned new_len = myArray->getLength() - itemset->getLength() + 1;
    	MyArray* new_array = new MyArray(new_len);

    	i = 0, j = 0;
    	bool flag = true;
    	while(j < myArray->getLength() && i < itemset->getLength()){
    		if(myArray->getData(j) < itemset->getEdgeId(i)){
    			if(flag){
					if(myArray->getData(j) < id_itemset){
						new_array->insert(myArray->getData(j));
					}
					else{
						new_array->insert(id_itemset);
						new_array->insert(myArray->getData(j));
						flag = false;
					}
    			}
    			else{
    				new_array->insert(myArray->getData(j));
    			}
    			j++;
    		}
    		else if(myArray->getData(j) == itemset->getEdgeId(i)){
    			j++;
    			i++;
    		}
    		else {//if(myArray->getData(j) > itemset->getEdgeId(i))
                cout << "wrong condition!" << outFile << endl;
                exit(EXIT_FAILURE);
    		}
    	}
    	assert(i == itemset->getLength());

    	//copy all the remaining data in myArray to new_array
    	if(flag){
			while(j < myArray->getLength()){
				if(flag){
					if(myArray->getData(j) < id_itemset) {
						new_array->insert(myArray->getData(j));
					}
					else{
						new_array->insert(id_itemset);
						new_array->insert(myArray->getData(j));
						flag = false;
					}
				}
				else{
					std::copy(myArray->getData() + j, myArray->getData() + myArray->getLength(), new_array->getData() + new_array->getIndex());
					break;
				}
				j++;
			}

			if(flag){
				new_array->insert(id_itemset);
			}
    	}
    	else{
    		copy(myArray->getData() + j, myArray->getData() + myArray->getLength(), new_array->getData() + new_array->getIndex());
    	}

    	//update myArray
    	myArray->shallowCopy(new_array);
    	delete new_array;
    	return true;
    }

//    //shallow copy
//    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph* graph) {
//    	cout << "shouldn't call this function!" << endl;
//    	exit (EXIT_FAILURE);
//    }

    static void update_parallel(ItemsetGraphStore* current, NaiveGraphStore* another, Concurrent_Worklist<vertexid_t>* worklist){
    	//for debugging
    	Logger::print_thread_info_locked("update-parallel starting...\n", LEVEL_LOG_FUNCTION);

    	vertexid_t id = -1;
    	while(worklist->pop_atomic(id)){
//    		//for debugging
//    		Logger::print_thread_info_locked("graph id = " + to_string(id) + "\n", LEVEL_LOG_FUNCTION);

    		assert(current->graphs.find(id) != current->graphs.end());
    		current->update_convert_locked(id, another->getMap().at(id));
    	}

    	//for debugging
    	Logger::print_thread_info_locked("update-parallel finished.\n", LEVEL_LOG_FUNCTION);
    }

    void update_graphs_parallel(GraphStore* another){
    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel starting...\n", LEVEL_LOG_FUNCTION);

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

    	//for debugging
    	Logger::print_thread_info_locked("update-graphs-parallel finished.\n", LEVEL_LOG_FUNCTION);
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

    void printOutInfo(){
    	int size_graphs = graphs.size();
    	long size_edges = 0;

    	cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    	for(auto it = graphs.begin(); it != graphs.end(); ++it){
    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
    		size_edges += it->second->getNumEdges();
    	}

    	cout << "Number of graphs: " << size_graphs << endl;
    	cout << "Number of edges: " << size_edges << endl;
    }


    /*
     * write graphs into a file for itemset mining later where
     * each graph as a line separated by " "
     */
    void writeToFile() {
		ofstream myfile;
		myfile.open(inputFile, std::ofstream::out);
//		//for debugging
//		cout << inputFile << endl;
//		cout << myfile.is_open() << endl;

		if (myfile.is_open()) {
			for (auto &n : graphs) {
//				n.second->write_for_mining(myfile);
				if(!n.second->isEmpty()){
					for(unsigned int i = 0; i < n.second->getLength(); i++){
						myfile << n.second->getEdgeId(i) << " ";
					}
					myfile << "\n";
				}
			}
			myfile.close();
		}
    }

    //
    void readFromFile() {
        ifstream myfile;
        myfile.open(outFile);
        if (!myfile) {
            cout << "can't load file: " << outFile << endl;
//            exit(EXIT_FAILURE);
        }
        else{
			//get all the frequent itemset and the corresponding frequency info
			multimap<int, ItemsetGraph*> frequency_graph_map;
			string line;
			while (getline(myfile, line)) {
				if(line == ""){
					continue;
				}

				set<int> itemset;
				int frequency;
				std::stringstream stream(line);
				do{
					std::string id = "";
					stream >> id;

					if(id == ""){
						continue;
					}

					if(id[0] != '('){
						itemset.insert(atoi(id.c_str()));
					}
					else {
						string word = "";
						for(auto c: id){
							if(c != '(' && c != ')'){
								word = word + c;
							}
						}
						frequency = atoi(word.c_str());
					}
				}
				while(stream);

				//transfer itemset into array
				ItemsetGraph* g = new ItemsetGraph(itemset);
				//sort itemsets in the ascending order of frequency
				frequency_graph_map.insert(std::pair<int, ItemsetGraph*>(frequency, g));
			}
			myfile.close();


	//		//filter to obtain top-k disjoint frequent itemsets
	//		int k = 5;
	//		get_disjoint_itemset(frequency_graph_map, k);

			for(auto it = frequency_graph_map.cbegin(); it != frequency_graph_map.cend(); ++ it){
				intToItemset.push_back((*it).second);

//				//for debugging
//				cout << *((*it).second) << endl;
			}
        }
    }

//    void get_disjoint_itemset(multimap<int, ItemsetGraph*>& frequency_graph_map, int k){
//
//    }


    /*
     *mine a set of frequent itemsets to construct the itemset base for encoding
     *precondition: the current intToItemset is empty
     *              or would like to reconstruct the base from scratch
     */
    void constructItemsetBase(){
    	if(!graphs.empty()){
			writeToFile();

			std::string option = "-tc -s30 -m10";
			std::string command = "../lib/eclat " + option + " " + inputFile + " " + outFile;
			system(command.c_str());

			readFromFile();
    	}
    }

    /*
     *update the existing itemset base by adding extra frequent itemsets
     */
    void updateItemsetBase(){
    	//TODO

    }

    /*
     * reconstruct the entire itemset base
     */
    void reconstructItemsetBase(){

    }


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
