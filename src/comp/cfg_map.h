/*
 * cfg_map.h
 *
 *  Created on: Jun 11, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_H_
#define COMP_CFG_MAP_H_

#include "cfg.h"

class CFG_map : public CFG {

public:
    CFG_map();


    ~CFG_map(){
        for (auto &node : nodes) {
            delete node;
        }
    }

    std::vector<CFGNode*> getPredesessors(const CFGNode* node) const override{
        auto it = predes.find(node);
        if(it != predes.end()){
            return it->second;
        }
        else{
            perror("invalid key!");
//			return NULL;
        }
    }

    std::vector<CFGNode*> getSuccessors(const CFGNode* node) const override {
        auto it = succes.find(node);
        if(it != succes.end()){
            return it->second;
        }
        else{
            perror("invalid key!");
//			return NULL;
        }
    }

    std::vector<CFGNode*> getNodes() const override {
        return nodes;
    }

    void addOneNode(CFGNode *Node) override {
        nodes.push_back(Node);
    }


    void addOnePred(CFGNode *succ, CFGNode *pred) override {
       predes[succ].push_back(pred);
    }

    void addOneSucc(CFGNode *pred, CFGNode *succ) override {
        succes[pred].push_back(succ);
    }

private:

    std::vector<CFGNode*> nodes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;

};



#endif /* COMP_CFG_MAP_H_ */
