/*
 * cfg_node.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_NODE_ALIAS_H_
#define COMP_CFG_NODE_ALIAS_H_

//#include "peGraphPointer.h"
//#include "../common/CommonLibs.hpp"
//#include "stmt/stmt.h"
//#include "stmt/stmt_alloc.h"
//#include "stmt/stmt_assign.h"
//#include "stmt/stmt_store.h"
//#include "stmt/stmt_load.h"
//#include "stmt/stmt_phi.h"
//#include "stmt/stmt_skip.h"
//#include "stmt/stmt_ret.h"
//#include "stmt/stmt_return.h"
//#include "stmt/stmt_call.h"
//#include "stmt/stmt_callfptr.h"
//#include "stmt/stmt_calleefptr.h"
//#include "graphstore/buffer.h"
#include "cfg_node.h"

using namespace std;

class CFGNode_alias: public CFGNode{

	friend std::ostream & operator<<(std::ostream & strm,  CFGNode_alias& cfgNode) {
		strm << cfgNode.getOutPointer();
		if(cfgNode.getStmt()){
			strm << cfgNode.getStmt()->toString();
		}
		return strm;
	}

public:
    CFGNode_alias(){
    	id = -1;
    	stmt = NULL;
    }

    CFGNode_alias(int _id, Stmt* _stmt) {
    	id = _id;
    	stmt = _stmt;
    }

    CFGNode_alias(std::string& line){
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
			stmt = new CallStmt(stream);
		}
		else if (type == "return") {
			stmt = new ReturnStmt(stream);
		}
		else if (type == "ret") {
			stmt = new RetStmt();
		}
		else if (type == "block") {
			stmt = new SkipStmt();
		}
		else if(type == "callfptr"){
			stmt = new CallfptrStmt(stream);
		}
		else if(type == "calleefptr"){
			stmt = new CalleefptrStmt(stream);
		}
		else {
			cout << "wrong stmt type!!!" << endl;
			exit(EXIT_FAILURE);
		}
	}

    ~CFGNode_alias(){
    	if(stmt){
			delete stmt;
    	}
    }


    size_t get_size_bytes() const {
    	return sizeof(PEGraph_Pointer) + sizeof(TYPE) + stmt->get_size_bytes();
    }

    void write_to_buf(Buffer& buf){
    	size_t s = get_size_bytes();
    	memcpy(buf.getData() + buf.getSize(), (char*)& s, sizeof(size_t));
    	buf.add_size_by(sizeof(size_t));
    	memcpy(buf.getData() + buf.getSize(), (char*)& id, sizeof(PEGraph_Pointer));
    	buf.add_size_by(sizeof(PEGraph_Pointer));
    	TYPE t = stmt->getType();
    	memcpy(buf.getData() + buf.getSize(), (char*)& t, sizeof(TYPE));
    	buf.add_size_by(sizeof(TYPE));
    	stmt->write_to_buf(buf);
    }

    void read_from_buf(char* buf, size_t bufsize){
    	size_t offset = 0;
    	id = *((PEGraph_Pointer*)(buf));
    	offset += sizeof(PEGraph_Pointer);
    	TYPE type = *((TYPE*)(buf + offset));
    	offset += sizeof(TYPE);

    	switch(type){
    	case TYPE::Assign: stmt = new AssignStmt(); break;
    	case TYPE::Load: stmt = new LoadStmt(); break;
    	case TYPE::Store: stmt = new StoreStmt(); break;
    	case TYPE::Alloca: stmt = new AllocStmt(); break;
    	case TYPE::Phi: stmt = new PhiStmt(); break;
    	case TYPE::Call: stmt = new CallStmt(); break;
    	case TYPE::Return: stmt = new ReturnStmt(); break;
    	case TYPE::Ret: stmt = new RetStmt(); break;
    	case TYPE::Skip: stmt = new SkipStmt(); break;
    	case TYPE::Callfptr: stmt = new CallfptrStmt(); break;
    	case TYPE::Calleefptr: stmt = new CalleefptrStmt(); break;
    	default:
    		cout << "wrong stmt type!!!" << endl;
    		exit(EXIT_FAILURE);
    	}

    	stmt->read_from_buf(buf, offset, bufsize);
    }

    inline Stmt* getStmt() const {
        return stmt;
    }

//	inline PEGraph_Pointer getInPointer() const {
//		return in_pointer;
//	}

    
private:
    //PEGraph_Pointer id;

    Stmt* stmt = nullptr;


};



#endif /* COMP_CFG_NODE_H_ */



