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

using namespace std;

class partition_info{
public:
	partition_info(unsigned int id, int s){
		this->partition_id = id;
		this->score = s;
	}

//private:
	unsigned int partition_id;
	int score;

};


struct partition_compare {
    bool operator() (const partition_info& lhs, const partition_info& rhs) const {
        return lhs.score < rhs.score;
    }
};


class Context {

public:
	Context(unsigned int n_parts, long total_nodes){
		this->number_partitions = n_parts;
		this->total_nodes = total_nodes;
		this->partitions = new unsigned int[this->number_partitions];
		for(unsigned int i = 0; i < this->number_partitions; i++){
			this->priority_set.insert(partition_info(i, 0));
		}
	}

	~Context(){
		delete[] partitions;
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

//	static const string&& getFileCfg() const {
//		return file_cfg;
//	}
//
//	static const string&& getFileGrammar() const {
//		return file_grammar;
//	}
//
//	static const string&& getFileSingletons() const {
//		return file_singletons;
//	}
//
//	static const string&& getFileStmts() const {
//		return file_stmts;
//	}

	unsigned int getNumberPartitions() const {
		return number_partitions;
	}

//	unsigned int* getPartitions() const {
//		return partitions;
//	}

//	const std::set<partition_info, partition_compare>& getPrioritySet() const {
//		return priority_set;
//	}

	long getTotalNodes() const {
		return total_nodes;
	}

//	static const string&& getWorkingPath() const {
//		return working_path;
//	}


	Partition schedule(){
		return 0;
	}

private:

	long total_nodes;
	unsigned int number_partitions;
	unsigned int* partitions;

	std::set<partition_info, partition_compare> priority_set;


};




#endif /* COMP_CONTEXT_H_ */
