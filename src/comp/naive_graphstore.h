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
    	for(auto & it : map){
    		delete it.second;
    	}
    }


    static const int level_log_function = 1;


    //deep copy
    PEGraph* retrieve(PEGraph_Pointer graph_pointer) {
//    	std::unique_lock < std::mutex > lock(mutex);

    	//for debugging
    	Logger::print_thread_info_locked("retrieve starting...\n", level_log_function);

    	PEGraph* out;

    	if(map.find(graph_pointer) != map.end()){
    		out = new PEGraph(map[graph_pointer]);
    	}
    	else{
    		out = new PEGraph();
    	}

    	//for debugging
    	Logger::print_thread_info_locked("retrieve finished.\n", level_log_function);

    	return out;
    }

    //deep copy
    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//    	std::unique_lock < std::mutex > lock(mutex);

    	//for debugging
    	Logger::print_thread_info_locked("update starting...\n", level_log_function);

//    	assert(map.find(graph_pointer) != map.end());
    	if(map.find(graph_pointer) != map.end()){
//			delete map[graph_pointer];
    	}
		map[graph_pointer] = new PEGraph(pegraph);

    	//for debugging
    	Logger::print_thread_info_locked("update finished.\n", level_log_function);
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
