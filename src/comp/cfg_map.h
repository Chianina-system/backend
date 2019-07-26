/*
 * cfg_map.h
 *
 *  Created on: Jun 11, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_H_
#define COMP_CFG_MAP_H_

#include "cfg.h"
using namespace std;

class CFG_map : public CFG {

//	friend std::ostream & operator<<(std::ostream & strm, const CFG_map& cfg){
//		cfg.print(strm);
//		return strm;
//	}

public:
    CFG_map(){}

//    CFG_map(std::string file){
//		std::ifstream fin;
//		fin.open(file);
//		if (!fin) {
//			cout << "can't load file_cfg: " << file << endl;
//		}
//    }

    CFG_map(const string& file_cfg, const string& file_stmt){

    }

    ~CFG_map(){
        for (auto &node : nodes) {
            delete node;
        }
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

			CFGNode* cfgNode = new CFGNode(line);
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

			//        cfgMap->addOneNode(m[atoi(pred_id.c_str())]);
			//        cfgMap->addOneNode(m[atoi(succ_id.c_str())]);
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

//			//add entry node
			this->nodes_entry.push_back(m[id]);
		}
		fin.close();

    }

    std::vector<CFGNode*> getPredesessors(const CFGNode* node) const override{
        auto it = predes.find(node);
        if(it != predes.end()){
            return it->second;
        }
        else{
        	return std::vector<CFGNode*>();
//            perror("invalid key!");
//            exit (EXIT_FAILURE);
        }
    }

    std::vector<CFGNode*> getSuccessors(const CFGNode* node) const override {
        auto it = succes.find(node);
        if(it != succes.end()){
            return it->second;
        }
        else{
        	return std::vector<CFGNode*>();
//            perror("invalid key!");
//            exit (EXIT_FAILURE);
        }
    }

    std::vector<CFGNode*> getNodes() const override {
        return nodes;
    }

    std::vector<CFGNode*> getEntryNodes() const {
    	return nodes_entry;
    }


    void addOneNode(CFGNode *Node)  {
        nodes.push_back(Node);
    }


    void addOnePred(CFGNode *succ, CFGNode *pred)  {
		if(predes.find(succ) == predes.end()) {
			predes[succ] = std::vector<CFGNode*>();
		}
		predes[succ].push_back(pred);
    }

    void addOneSucc(CFGNode *pred, CFGNode *succ)  {
    	if(succes.find(pred) == succes.end()){
    		succes[pred] = std::vector<CFGNode*>();
    	}
		succes[pred].push_back(succ);
		this->number_edges++;
    }

    vertexid_t getNumberEdges() const {
    	return number_edges;
    }

    void print(std::ostream& str) const override {
    	str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
    	str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
    	for(auto it = this->succes.begin(); it != this->succes.end(); ++it){
    		const CFGNode* node = it->first;
    		for(auto& n: it->second){
    			str << *node << "\t" << *n << "\n";
    		}
    	}
    }


protected:

    std::vector<CFGNode*> nodes;

    std::vector<CFGNode*> nodes_entry;

    vertexid_t number_edges = 0;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;



};



#endif /* COMP_CFG_MAP_H_ */
