/*
 * NaiveGraphStore.h
 *
 *  Created on: Jun 27, 2019
 *      Author: zqzuo
 */

#ifndef COMP_NAIVE_GRAPHSTORE_H_
#define COMP_NAIVE_GRAPHSTORE_H_

#include "graphstore.h"
using namespace std;

class NaiveGraphStore : public GraphStore {

public:
	NaiveGraphStore(){}

	NaiveGraphStore(const string& file_singleton){

	}

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



    //deep copy
    PEGraph* retrieve(PEGraph_Pointer graph_pointer) {
    	if(map.find(graph_pointer) != map.end()){
    		return new PEGraph(map[graph_pointer]);
    	}
    	map[graph_pointer] = new PEGraph();
    	return new PEGraph();
    }

    //deep copy
    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) {
//    	assert(map.find(graph_pointer) != map.end());
    	if(map.find(graph_pointer) != map.end()){
			delete map[graph_pointer];
    	}
		map[graph_pointer] = new PEGraph(pegraph);
    }


    void print(std::ostream& str) const override {
    	str << "The singleton set: [" ;
    	for(auto & id : singletonSet){
    		str << id << ", ";
    	}
    	str << "]\n";

    	str << "The number of graphs is: " << map.size() << "\n";
    	for(auto it = map.begin(); it != map.end(); ++it){
    		cout << "======================" << endl;
    		str << it->first << "--> " << *(it->second) << endl;
    	}
//    	for(auto & graphpair: map){
//    		cout << "======================" << endl;
//    		str << graphpair.first << "--> " << *(graphpair.second) << endl;
//    	}
    }


private:
	std::unordered_map<PEGraph_Pointer, PEGraph*> map;

};



#endif /* COMP_NAIVE_GRAPHSTORE_H_ */
