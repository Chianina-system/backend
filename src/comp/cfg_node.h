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

	friend std::ostream & operator<<(std::ostream & strm, const CFGNode& cfgNode) {
		strm << cfgNode.getOutPointer();
		return strm;
	}

public:
    CFGNode();
    CFGNode(int _id, Stmt* _stmt) {
    id = _id;
    stmt = _stmt;
    }

    ~CFGNode(){}


    inline Stmt* getStmt(){
        return stmt;
    }

//	inline PEGraph_Pointer getInPointer() const {
//		return in_pointer;
//	}

    inline PEGraph_Pointer getOutPointer() const {
        return id;
    }

    inline int getCfgNodeId() const {
        return id;
    }

    inline void setCfgNodeId(int cfgNodeId) {
        id = cfgNodeId;
    }


private:
    PEGraph_Pointer id;
//    int id;

    Stmt* stmt;

    //vetex id...


};



#endif /* COMP_CFG_NODE_H_ */
