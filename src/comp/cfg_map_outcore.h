/*
 * cfg_map_outcore.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_H_
#define COMP_CFG_MAP_OUTCORE_H_

#include "cfg_map.h"

class CFG_map_outcore : public CFG_map {


public:
	CFG_map_outcore(){}

	~CFG_map_outcore(){

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

		std::map<int, CFGNode*> m;

		std::string line;
		while (getline(fin, line) && line != "") {
			std::cout << line << "\n";

			std::stringstream stream(line);
			std::string stmt_id, type, dst, src, added;
			stream >> stmt_id >> type >> dst >> src >> added;

			std::cout << stmt_id << "," << type << "," << dst << "," << src
					<< "," << added << "\n";

			TYPE t;
			if (type == "assign") {
				t = TYPE::Assign;
			}
			if (type == "load") {
				t = TYPE::Load;
			}
			if (type == "store") {
				t = TYPE::Store;
			}
			if (type == "alloca") {
				t = TYPE::Alloca;
			}

			Stmt* stmt = new Stmt(t, atoi(src.c_str()), atoi(dst.c_str()),
					atoi(added.c_str()));
			CFGNode* cfgNode = new CFGNode(atoi(stmt_id.c_str()), stmt);
			m[atoi(stmt_id.c_str())] = cfgNode;

			//add cfgnode into cfg
			this->addOneNode(cfgNode);

			//add entry node
			if(cfgNode->getCfgNodeId() == 0){
				this->nodes_entry.push_back(cfgNode);
			}
		}
		fin.close();

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

		//handle mirrors file
		fin.open(file_mirrors_in);
		if (!fin) {
			cout << "can't load file_mirrors_in: " << file_mirrors_in << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line) && line != "") {
			std::stringstream stream(line);
			std::string item;
			while(getline(stream, item, '\t')){
				this->in_mirrors.insert(m[atoi(item.c_str())]);
			}
		}
		fin.close();

		fin.open(file_mirrors_out);
		if (!fin) {
			cout << "can't load file_mirrors_out: " << file_mirrors_out << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line) && line != "") {
			std::stringstream stream(line);
			std::string item;
			while(getline(stream, item, '\t')){
				this->out_mirrors.insert(m[atoi(item.c_str())]);
			}
		}
		fin.close();

		//handle actives file
		fin.open(file_actives);
		if (!fin) {
			cout << "can't load file_actives: " << file_actives << endl;
			exit(EXIT_FAILURE);
		}

		while (getline(fin, line) && line != "") {
			std::stringstream stream(line);
			std::string item;
			while(getline(stream, item, '\t')){
				this->actives.insert(m[atoi(item.c_str())]);
			}
		}
		fin.close();

	}



private:
	std::unordered_set<CFGNode*> in_mirrors;

	std::unordered_set<CFGNode*> out_mirrors;

	std::unordered_set<CFGNode*> actives;



};



#endif /* COMP_CFG_MAP_OUTCORE_H_ */
