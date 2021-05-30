/*
 * cfg_node.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_NODE_H_
#define COMP_CFG_NODE_H_

//#include "peGraphPointer.h"
#include "../common/CommonLibs.hpp"
#include "stmt/stmt.h"
#include "stmt/stmt_alloc.h"
#include "stmt/stmt_assign.h"
#include "stmt/stmt_store.h"
#include "stmt/stmt_load.h"
#include "stmt/stmt_phi.h"
#include "stmt/stmt_skip.h"
#include "stmt/stmt_ret.h"
#include "stmt/stmt_return.h"
#include "stmt/stmt_call.h"
#include "stmt/stmt_callfptr.h"
#include "stmt/stmt_calleefptr.h"
#include "graphstore/buffer.h"


using namespace std;

class CFGNode{

public:

    CFGNode(){
    	id = -1;
    	//len1= len;
    	// content = new int[len1];


    	//stmt = NULL;
    }

    CFGNode(int _id) {
    	id = _id;
    }

    virtual ~CFGNode(){   };

    virtual size_t get_size_bytes() const = 0;

    virtual void write_to_buf(Buffer& buf) = 0;

    virtual void read_from_buf(char* buf, size_t bufsize) = 0;


    virtual inline Stmt* getStmt() const  = 0;

    inline PEGraph_Pointer getOutPointer() const {
        return id;
    }

    inline int getCfgNodeId() const {
        return id;
    }

    inline void setCfgNodeId(int cfgNodeId) {
        id = cfgNodeId;
    }


protected:
    PEGraph_Pointer id;

};


#endif /* COMP_CFG_NODE_H_ */


