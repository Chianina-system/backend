/*
 * NaiveGraphStore.h
 *
 *  Created on: Jun 27, 2019
 *      Author: zqzuo
 */

#ifndef COMP_NAIVE_GRAPHSTORE_H_
#define COMP_NAIVE_GRAPHSTORE_H_

#include "graphstore.h"
#include "../utility/Logger.hpp"
//#include "cfg_compute.h"

using namespace std;

class NaiveGraphStore : public GraphStore {

public:
	NaiveGraphStore(){}

    void loadGraphStore(const string& file_singleton) override {
	    //handle the singleton.txt
	    std::ifstream fin;
	    fin.open(file_singleton);
	    if(!fin) {
	        cout << "can't load file_singleton: " << file_singleton << endl;
	        exit (EXIT_FAILURE);
	    }

	    std::string line;
	    while (getline(fin, line) && line != "") {
	        std::stringstream stream(line);
	        std::string id;
	        stream >> id;
	        this->addOneSingleton(atoi(id.c_str()));
	    }
	    fin.close();
	}


    ~NaiveGraphStore(){
//    	for(auto & it : map){
//    		delete it.second;
//    	}
    	clear();
    }


    //deep copy
    PEGraph* retrieve(PEGraph_Pointer graph_pointer){
//    	std::unique_lock < std::mutex > lock(mutex);

    	//for debugging
    	Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

    	PEGraph* out;

    	if(map.find(graph_pointer) != map.end()){
    		Logger::print_thread_info_locked("retrieving +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
    		out = new PEGraph(map[graph_pointer]);
    	}
    	else{
    		out = new PEGraph();
    	}

    	//for debugging
    	Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

    	return out;
    }

    //deep copy
    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//    	std::unique_lock < std::mutex > lock(mutex);

    	//for debugging
    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

//    	assert(map.find(graph_pointer) != map.end());
    	if(map.find(graph_pointer) != map.end()){
			delete map[graph_pointer];
    		Logger::print_thread_info_locked("deleting +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
    	}
		map[graph_pointer] = new PEGraph(pegraph);

    	//for debugging
    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }

    void addOneGraph(PEGraph_Pointer pointer, PEGraph* graph){
    	this->map[pointer] = graph;
    }

    void update_graphs(GraphStore* another){
    	NaiveGraphStore* another_graphstore = dynamic_cast<NaiveGraphStore*>(another);
    	for(auto& it: another_graphstore->map){
    		update(it.first, it.second);
    	}
    }

    void clearEntryOnly(){
    	this->map.clear();
    }

    void clear(){
    	for(auto it = map.begin(); it != map.end(); ){
    		delete it->second;
    		it = map.erase(it);
    	}
    }

protected:
    void print(std::ostream& str) const override {
    	str << "The singleton set: [" ;
    	for(auto & id : singletonSet){
    		str << id << ", ";
    	}
    	str << "]\n";

    	str << "The number of graphs is: " << map.size() << "\n";
    	for(auto it = map.begin(); it != map.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

    void toString_sub(std::ostringstream& str) const override {
    	str << "The singleton set: [" ;
    	for(auto & id : singletonSet){
    		str << id << ", ";
    	}
    	str << "]\n";

    	str << "The number of graphs is: " << map.size() << "\n";
    	for(auto it = map.begin(); it != map.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }


private:
	std::unordered_map<PEGraph_Pointer, PEGraph*> map;

//	std::mutex mutex;

};



#endif /* COMP_NAIVE_GRAPHSTORE_H_ */
