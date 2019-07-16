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
#include "../utility/FileUtil.hpp"
//#include "cfg_compute.h"

using namespace std;

class NaiveGraphStore : public GraphStore {

public:
	NaiveGraphStore(){}


    ~NaiveGraphStore(){
    	clear();
    }


    void load_onebyone(const string& file){
    	//for debugging
    	Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);

    	cout << "file name: " << file << endl;

	    std::ifstream fin;
	    fin.open(file);
	    if(!fin) {
	        cout << "can't load file_graphs: " << file << endl;
//	        exit (EXIT_FAILURE);
	    }
	    else{
			std::string line;
			while (getline(fin, line) && line != "") {
				std::stringstream stream(line);
				std::string id;
				stream >> id;
				PEGraph_Pointer graph_pointer = atoi(id.c_str());
				PEGraph* pegraph = new PEGraph();
				pegraph->load_readable(stream);
				//since the file is appended, we just use the recent updated pegraph
				if (map.find(graph_pointer) != map.end()) {
					delete map[graph_pointer];
				}
				map[graph_pointer] = pegraph;
			}
			fin.close();

	    	//delete the old graphstore file
	    	FileUtil::delete_file(file);
	    }

		//for debugging
		Logger::print_thread_info_locked("load-readable finished.\n", LEVEL_LOG_FUNCTION);

    }


    void loadGraphStore(const string& file, const string& folder_in) {
    	//graphstore file
    	this->deserialize(file);

    	//updated graphstore
//        DIR* dirp = opendir(folder_in.c_str());
//        if(dirp){
//			struct dirent * dp;
//			while ((dp = readdir(dirp)) != NULL) {
//				if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
//					cout << "file name: " << dp->d_name << endl;
//					this->load_onebyone(folder_in + "/" + dp->d_name);
//				}
//			}
//			closedir(dirp);
//        }
//        else{
//        	cout << "can't load folder: " << folder_in << endl;
//        }

    	this->load_onebyone(folder_in);
    }


    void serialize(const string& file){
    	if(readable){
    		ofstream myfile;
    		myfile.open(file, std::ofstream::out);
    		if (myfile.is_open()){
    			for (auto& n : map) {
    				//write a pegraph into file
    		    	myfile << n.first << "\t";
    		    	n.second->write_readable(myfile);
    		    	myfile << "\n";
    			}
    			myfile.close();
    		}
    	}
    	else{

    	}
    }

    void deserialize(const string& file){
    	if(readable){
    		load_onebyone(file);
    	}
    	else{

    	}
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
