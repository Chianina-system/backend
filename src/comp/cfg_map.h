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
        for(auto it = nodes.begin(); it != nodes.end(); it++){
            delete *it;
        }
    }


    std::vector<CFGNode*> getPredesessors(const CFGNode* node) const{
        std::unordered_map<const CFGNode*, std::vector<CFGNode*>>::const_iterator it = predes.find(node);
        if(it != predes.end()){
            return it->second;
        }
        else{
            perror("invalid key!");
//			return NULL;
        }
    }

    std::vector<CFGNode*> getSuccessors(const CFGNode* node) const {
        std::unordered_map<const CFGNode*, std::vector<CFGNode*>>::const_iterator it = succes.find(node);
        if(it != succes.end()){
            return it->second;
        }
        else{
            perror("invalid key!");
//			return NULL;
        }
    }

    std::vector<CFGNode*> getNodes() const {
        return nodes;
    }



private:

    std::vector<CFGNode*> nodes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;



};



#endif /* COMP_CFG_MAP_H_ */
