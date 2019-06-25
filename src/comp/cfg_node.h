/*
 * cfg_node.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_NODE_H_
#define COMP_CFG_NODE_H_

#include "peGraphPointer.h"
#include "stmt.h"
#include "../common/CommonLibs.hpp"

class CFGNode{

public:
    CFGNode();
    CFGNode(int id, Stmt* _stmt) {
    cfg_node_id = id;
    stmt = _stmt;
    }

    ~CFGNode(){}


    inline Stmt* getStmt(){
        return stmt;
    }

//    inline int getCfgNodeId(){
//        return cfg_node_id;
//    }


//	inline PEGraph_Pointer getInPointer() const {
//		return in_pointer;
//	}

    inline PEGraph_Pointer getOutPointer() const {
        return out_pointer;
    }

    inline int getCfgNodeId() const {
        return cfg_node_id;
    }

    inline void setCfgNodeId(int cfgNodeId) {
        cfg_node_id = cfgNodeId;
    }


private:
//	PEGraph_Pointer in_pointer;
    PEGraph_Pointer out_pointer;
    int cfg_node_id;

    Stmt* stmt;

    //vetex id...


};

//CFGNode::CFGNode(int id, Stmt *_stmt) {
//    cfg_node_id = id;
//    stmt = _stmt;
//}



#endif /* COMP_CFG_NODE_H_ */
