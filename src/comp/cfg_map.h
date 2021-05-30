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

public:
    CFG_map(){}

    virtual ~CFG_map(){
        //        for (auto &node : nodes) {
        //            delete node;
        //        }
    }

    virtual void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_entries) = 0;


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


    std::vector<CFGNode*> getEntryNodes() const {
    	return nodes_entry;
    }

    void print(std::ostream& str) const override {
    	str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
    	str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
    }

    void addOneNode(CFGNode *Node)  {
        nodes.push_back(Node);
    }
protected:

    std::vector<CFGNode*> nodes_entry;


};



#endif /* COMP_CFG_MAP_H_ */

