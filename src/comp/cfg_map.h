/*
 * cfg_map.h
 *
 *  Created on: Jun 11, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_H_
#define COMP_CFG_MAP_H_

#include "cfg.h"

class CFG_map : CFG {

public:
    CFG_map();

    CFG_map(std::string file);


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



private:

    std::vector<CFGNode*> nodes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;



};

CFG_map::CFG_map(std::string file) {
    std::ifstream fin;
    fin.open(file);
    if(!fin) {
        cout << "can't load file_cfg: " << file << endl;
    }

}


#endif /* COMP_CFG_MAP_H_ */
