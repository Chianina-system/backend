/*
 * cfg_map_outcore.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_H_
#define COMP_CFG_MAP_OUTCORE_H_

#include "cfg.h"
#include "../utility/FileUtil.hpp"

class CFG_map_outcore : public CFG {


public:
	CFG_map_outcore(){}

	~CFG_map_outcore(){
        for (auto &node : nodes) {
            delete node;
        }
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

	inline bool isMirror(CFGNode* node) {
		return in_mirrors.find(node) != in_mirrors.end() || out_mirrors.find(node) != out_mirrors.end();
	}

	inline bool isInMirror(CFGNode* node) {
		return in_mirrors.find(node) != in_mirrors.end();
	}

	inline bool isOutMirror(CFGNode* node) {
		return out_mirrors.find(node) != out_mirrors.end();
	}

	inline std::unordered_set<CFGNode*>& getActiveNodes(){
		return actives;
	}


	void loadCFG_ooc(const string& file_cfg, const string& file_stmt, const string& file_mirrors_in, const string& file_mirrors_out, const string& file_actives) {
		// handle the stmt file
		std::ifstream fin;
		fin.open(file_stmt);
		if (!fin) {
			cout << "can't load file_stmt " << endl;
			exit(EXIT_FAILURE);
		}

		std::map<PEGraph_Pointer, CFGNode*> m;

		std::string line;
		while (getline(fin, line) && line != "") {
			CFGNode* cfgNode = new CFGNode(line);
			m[cfgNode->getCfgNodeId()] = cfgNode;

			//add cfgnode into cfg
			this->addOneNode(cfgNode);

//			//add entry node
//			if(cfgNode->getCfgNodeId() == 0){
//				this->nodes_entry.push_back(cfgNode);
//			}
		}
		fin.close();


		//handle mirrors file
		fin.open(file_mirrors_in);
		if (fin) {
			while (getline(fin, line) && line != "") {
	//			std::stringstream stream(line);
	//			std::string item;
	//			while(getline(stream, item, '\t')){
	//				this->in_mirrors.insert(m[atoi(item.c_str())]);
	//			}

				PEGraph_Pointer id = atoi(line.c_str());
				if(m.find(id) == m.end()){
					m[id] = new CFGNode(id, nullptr);
				}
				this->in_mirrors.insert(m[id]);
			}
			fin.close();
		}
		else{
			cout << "can't load file_mirrors_in: " << file_mirrors_in << endl;
//			exit(EXIT_FAILURE);
		}

		fin.open(file_mirrors_out);
		if (!fin) {
			cout << "can't load file_mirrors_out: " << file_mirrors_out << endl;
//			exit(EXIT_FAILURE);
		}
		else{
			while (getline(fin, line) && line != "") {
	//			std::stringstream stream(line);
	//			std::string item;
	//			while(getline(stream, item, '\t')){
	//				this->out_mirrors.insert(m[atoi(item.c_str())]);
	//			}

				PEGraph_Pointer id = atoi(line.c_str());
				if(m.find(id) == m.end()){
					m[id] = new CFGNode(id, nullptr);
				}
				this->out_mirrors.insert(m[id]);
			}
			fin.close();
		}


		//handle the cfg file
		fin.open(file_cfg);
		if (!fin) {
			cout << "can't load file_cfg: " << file_cfg << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line) && line != "") {
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



		//handle actives file
		fin.open(file_actives);
		if (!fin) {
			cout << "can't load file_actives: " << file_actives << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line) && line != "") {
//			std::stringstream stream(line);
//			std::string item;
//			while(getline(stream, item, '\t')){
//				this->actives.insert(m[atoi(item.c_str())]);
//			}
			this->actives.insert(m[atoi(line.c_str())]);
		}
		fin.close();

		//delete the actives file
		FileUtil::delete_file(file_actives);

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



private:
	std::unordered_set<CFGNode*> in_mirrors;

	std::unordered_set<CFGNode*> out_mirrors;

	std::unordered_set<CFGNode*> actives;



    std::vector<CFGNode*> nodes;

    vertexid_t number_edges = 0;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;



};



#endif /* COMP_CFG_MAP_OUTCORE_H_ */
