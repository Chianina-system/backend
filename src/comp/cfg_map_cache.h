/*
 * cfg_map.h
 *
 *  Created on: Jun 11, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_CACHE_H_
#define COMP_CFG_MAP_CACHE_H_

#include "cfg_map.h"
#include "cfg_node_cache.h"
using namespace std;

class CFG_map_cache : public CFG_map {

public:
    CFG_map_cache(){}

    ~CFG_map_cache(){
        //        for (auto &node : nodes) {
        //            delete node;
        //        }
    }

    void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_entries) {
        // std::map<vertexid_t, CFGNode*> m;
    }



  //  void addOneNode(CFGNode *Node)  {
  //      nodes.push_back(Node);
  //  }


};



#endif /* COMP_CFG_MAP_CACHE_H_ */

