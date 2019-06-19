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
    ~CFGNode(){}


    inline Stmt* getStmt(){
        return stmt;
    }

//	inline PEGraph_Pointer getInPointer() const {
//		return in_pointer;
//	}

    inline PEGraph_Pointer getOutPointer() const {
        return out_pointer;
    }


private:
//	PEGraph_Pointer in_pointer;
    PEGraph_Pointer out_pointer;


    Stmt* stmt;

    //vetex id...


};



#endif /* COMP_CFG_NODE_H_ */
