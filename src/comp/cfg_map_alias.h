/*
 * cfg_map.h
 *
 *  Created on: Jun 11, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_ALIAS_H_
#define COMP_CFG_MAP_ALIAS_H_

#include "cfg_map.h"
#include "cfg_node_alias.h"
using namespace std;

class CFG_map_alias: public CFG_map{

public:
    CFG_map_alias(){}

    ~CFG_map_alias(){
        //for (auto &node : nodes) {
        //    delete node;
        //}
    }


    void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_entries) {
		std::map<vertexid_t, CFGNode*> m;

		// handle the stmt file
		std::ifstream fin;
		fin.open(file_stmt);
		if (!fin) {
			cout << "can't load file_stmt " << file_stmt << endl;
			exit(EXIT_FAILURE);
		}

		std::string line;
		while (getline(fin, line)) {
			if(line == ""){
				continue;
			}

			CFGNode* cfgNode = new CFGNode_alias(line);
			assert(m.find(cfgNode->getCfgNodeId()) == m.end());
			m[cfgNode->getCfgNodeId()] = cfgNode;

			//add cfgnode into cfg
			this->addOneNode(cfgNode);
		}
		fin.close();

		//handle the cfg.txt
		fin.open(file_cfg);
		if (!fin) {
			cout << "can't load file_cfg: " << file_cfg << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line)) {
			if(line == ""){
				continue;
			}

			std::stringstream stream(line);
			std::string pred_id, succ_id;
			stream >> pred_id >> succ_id;

			this->addOneSucc(m[atoi(pred_id.c_str())],
					m[atoi(succ_id.c_str())]);
			this->addOnePred(m[atoi(succ_id.c_str())],
					m[atoi(pred_id.c_str())]);
		}
		fin.close();

		//handle entry nodes
		fin.open(file_entries);
		if (!fin) {
			cout << "can't load file_entries" << file_entries << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line)) {
			if(line == ""){
				continue;
			}

			vertexid_t id = atoi(line.c_str());
			assert(m.find(id) != m.end());

			//add entry node
			this->nodes_entry.push_back(m[id]);
		}
		fin.close();

    }

    
    void print(std::ostream& str) const override {
    	str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
    	str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
    	for(auto it = this->succes.begin(); it != this->succes.end(); ++it){
    	       //	const CFGNode* node = it->first;
    		for(auto& n: it->second){
    		 //	str << *node << "\t" << *n << "\n";
    		}
    	}
    }


};



#endif /* COMP_CFG_MAP_ALIAS_H_ */



