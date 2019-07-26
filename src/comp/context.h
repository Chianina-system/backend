/*
 * context.h
 *
 *  Created on: Jul 12, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONTEXT_H_
#define COMP_CONTEXT_H_

#include "../common/CommonLibs.hpp"
#include "../comp/cfg_node.h"
#include "grammar.h"

using namespace std;

class partition_info {

public:

//	partition_info(unsigned int id, int s){
//		this->partition_id = id;
//		this->score = s;
//	}

	partition_info(unsigned int id, int s = 0){
		this->partition_id = id;
		this->score = s;
	}

	//operator for map
	bool operator==(const partition_info& other) const {
		return this->partition_id == other.partition_id;
	}

	void increase_score(int s){
		this->score += s;
	}


//private:
	unsigned int partition_id;
	int score;

};

namespace std {
	template<>
	struct hash<partition_info> {
		std::size_t operator()(const partition_info& pinfo) const {
			//simple hash
			return std::hash<int>()(pinfo.partition_id);
		}
	};
}

struct partition_compare {
    bool operator() (const partition_info& lhs, const partition_info& rhs) const {
        return lhs.score < rhs.score;
    }
};


class Context {

public:
	Context(unsigned int n_parts, const string& file_total, const string& file_cfg, const string& file_stmts, const string& file_entries, const string& file_singletons, const string& file_grammar){
		this->number_partitions = n_parts;
		this->total_nodes = readTotalNodes(file_total);
		assert(this->total_nodes != -1);

//		this->partitions = new unsigned int[this->number_partitions];
//		this->flag_partitions = new bool[this->number_partitions];
//		for(unsigned int i = 0; i < this->number_partitions; i++){
//			flag_partitions[i] = false;
//		}

		//initialize the worklist
//		for(unsigned int i = 0; i < this->number_partitions; i++){
//			this->priority_set.insert(partition_info(i, 0));
//		}

		//file path initialization
		this->file_cfg_init = file_cfg;
		this->file_stmts_init = file_stmts;
		this->file_entries_init = file_entries;
		this->file_grammar_init = file_grammar;
		this->file_singletons_init = file_singletons;

		grammar = new Grammar();
		grammar->loadGrammar(file_grammar.c_str());
	}

	~Context(){
//		if(partitions){
//			delete[] partitions;
//		}
//
//		if(flag_partitions){
//			delete[] flag_partitions;
//		}

		if(grammar)
			delete grammar;
	}


	static const string working_path;
	static const string file_cfg;
	static const string file_stmts;
	static const string file_singletons;
	static const string file_grammar;
	static const string folder_actives;
	static const string file_graphstore;
	static const string folder_mirrors_in;
	static const string folder_mirrors_out;
	static const string folder_graphs_in;


	Partition getPartition(CFGNode* node){
		return getPartition(node->getCfgNodeId());
	}

	Partition getPartition(vertexid_t node_id){
		int mode = this->total_nodes / this->number_partitions + 1;
		return node_id / mode;
	}


	unsigned int getNumberPartitions() const {
		return number_partitions;
	}


	const std::set<partition_info, partition_compare>& getPrioritySet() const {
		return priority_set;
	}

	long getTotalNodes() const {
		return total_nodes;
	}

	Grammar* getGrammar() const {
		return grammar;
	}

	bool schedule(Partition& part){
		if(priority_set.empty()){
			return false;
		}
		else{
			auto first = priority_set.begin();
			part = first->partition_id;
			priority_set.erase(first);
			return true;
		}
	}

	void update_priority(Partition part, int size){
		partition_info pinfo(part, size);
		auto it = priority_set.find(pinfo);
		if(it != priority_set.end()){
//			assert(it != priority_set.end());
			int old_score = ((partition_info) (*it)).score;
			priority_set.erase(it);
			pinfo.increase_score(old_score);
			priority_set.insert(pinfo);
		}
		else{
			priority_set.insert(pinfo);
		}
	}

//	void reset_priority(Partition part){
//		partition_info pinfo(part);
//		auto it = priority_set.find(pinfo);
//		assert(it != priority_set.end());
//		priority_set.erase(it);
//	}

	string getFileCfg() const {
		return file_cfg_init;
	}

	string getFileGrammar() const {
		return file_grammar_init;
	}

	string getFileEntries() const {
		return file_entries_init;
	}

	string getFileSingletons() const {
		return file_singletons_init;
	}

	string getFileStmts() const {
		return file_stmts_init;
	}

//	int get_score(Partition pid){
//		partition_info pinfo(pid);
//		auto it = priority_set.find(pinfo);
//		assert(it != priority_set.end());
//		return ((partition_info)(*it)).score;
//	}

//	bool getFlag(Partition p){
//		return flag_partitions[p];
//	}
//
//	void setFlag(Partition p){
//		flag_partitions[p] = true;
//	}


private:

	long total_nodes;
	unsigned int number_partitions;
//	unsigned int* partitions;
//	bool* flag_partitions;

	std::set<partition_info, partition_compare> priority_set;

	/* TODO: load grammar from file grammar->loadGrammar(filename) */
	Grammar* grammar;

	//the path of input files
	string file_cfg_init;
	string file_stmts_init;
	string file_entries_init;
	string file_singletons_init;
	string file_grammar_init;


	long readTotalNodes(const string& file){
		ifstream myfile_total(file);
		if (myfile_total.is_open()) {
			string line;
			while (getline(myfile_total, line)) {
				if(line == ""){
					continue;
				}
				return atol(line.c_str());
			}

			myfile_total.close();
		}

		return -1;
	}

};




#endif /* COMP_CONTEXT_H_ */
