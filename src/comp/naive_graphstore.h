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



    ~NaiveGraphStore(){
    	clear();
    }


    void loadGraphStore(const string& file, const string& folder_in) {
    	//graphstore file
    	this->deserialize(file);

    	//updated graphstore
        DIR* dirp = opendir(folder_in.c_str());
        struct dirent * dp;
        while ((dp = readdir(dirp)) != NULL) {
        	this->deserialize(dp->d_name);
        }
        closedir(dirp);
    }


    void serialize(const string& file){

    }

    void deserialize(const string& file){

    }


    //deep copy; locked version for asynchronous mode
    PEGraph* retrieve_locked(PEGraph_Pointer graph_pointer){
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	return retrieve(graph_pointer);
    }

    //deep copy
    PEGraph* retrieve(PEGraph_Pointer graph_pointer){
//    	//for debugging
//    	Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

    	PEGraph* out;

    	if(map.find(graph_pointer) != map.end()){
//    		Logger::print_thread_info_locked("retrieving +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
    		out = new PEGraph(map[graph_pointer]);
    	}
    	else{
    		out = new PEGraph();
    	}

    	//for debugging
//    	Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

    	return out;
    }

    //deep copy; locked version for asynchronous mode
    void update_locked(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	update(graph_pointer, pegraph);
    }

    //deep copy
    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//    	//for debugging
//    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

//    	assert(map.find(graph_pointer) != map.end());
    	if(map.find(graph_pointer) != map.end()){
//    		Logger::print_thread_info_locked("deleting +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
			delete map[graph_pointer];
    	}
		map[graph_pointer] = new PEGraph(pegraph);

    	//for debugging
//    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
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
    void print(std::ostream& str) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	str << "The number of graphs is: " << map.size() << "\n";
    	for(auto it = map.begin(); it != map.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }

    void toString_sub(std::ostringstream& str) {
    	std::lock_guard<std::mutex> lockGuard(mutex);
    	str << "The number of graphs is: " << map.size() << "\n";
    	for(auto it = map.begin(); it != map.end(); ++it){
    		str << ">>>>" << it->first << " " << *(it->second) << endl;
    	}
    }


private:
	std::unordered_map<PEGraph_Pointer, PEGraph*> map;



};



#endif /* COMP_NAIVE_GRAPHSTORE_H_ */
