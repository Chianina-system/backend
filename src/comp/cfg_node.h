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

    CFGNode(std::string& line){
		std::stringstream stream(line);
		std::string stmt_id, type;
		stream >> stmt_id >> type;

		this->id = atoi(stmt_id.c_str());
		if (type == "assign") {
			stmt = new AssignStmt(stream);
		}
		else if (type == "load") {
			stmt = new LoadStmt(stream);
		}
		else if (type == "store") {
			stmt = new StoreStmt(stream);
		}
		else if (type == "alloca") {
			stmt = new AllocStmt(stream);
		}
		else if (type == "phi") {
			stmt = new PhiStmt(stream);
		}
		else if (type == "call") {
			stmt = new CallStmt();
		}
		else if (type == "return") {
			stmt = new ReturnStmt();
		}
		else if (type == "ret") {
			stmt = new RetStmt();
		}
		else if (type == "block") {
			stmt = new SkipStmt();
		}
		else {
			cout << "wrong stmt type!!!" << endl;
			exit(EXIT_FAILURE);
		}
	}

    ~CFGNode(){
    	if(stmt){
			delete stmt;
    	}
    }


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

    Stmt* stmt = nullptr;


};



#endif /* COMP_CFG_NODE_H_ */
