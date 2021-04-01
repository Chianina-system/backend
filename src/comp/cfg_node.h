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

//内存中IR信息
using namespace std;

class CFGNode{

	friend std::ostream & operator<<(std::ostream & strm, const CFGNode& cfgNode) {
		strm << cfgNode.getOutPointer();
//		if(cfgNode.getStmt()){
//			strm << cfgNode.getStmt()->toString();
//		}
		return strm;
	}

public:
    CFGNode(){
    	id = -1;
    	stmt = NULL;
    }

    CFGNode(int _id, Stmt* _stmt) {
    	id = _id;
    	stmt = _stmt;
    }

//    CFGNode(std::string& line){
//		std::stringstream stream(line);
//		std::string stmt_id, type;
//		stream >> stmt_id >> type;
//
//		this->id = atoi(stmt_id.c_str());
//		if (type == "assign") {
//			stmt = new AssignStmt(stream);
//		}
//		else if (type == "load") {
//			stmt = new LoadStmt(stream);
//		}
//		else if (type == "store") {
//			stmt = new StoreStmt(stream);
//		}
//		else if (type == "alloca") {
//			stmt = new AllocStmt(stream);
//		}
//		else if (type == "phi") {
//			stmt = new PhiStmt(stream);
//		}
//		else if (type == "call") {
//			stmt = new CallStmt(stream);
//		}
//		else if (type == "return") {
//			stmt = new ReturnStmt(stream);
//		}
//		else if (type == "ret") {
//			stmt = new RetStmt();
//		}
//		else if (type == "block") {
//			stmt = new SkipStmt();
//		}
//		else if(type == "callfptr"){
//			stmt = new CallfptrStmt(stream);
//		}
//		else if(type == "calleefptr"){
//			stmt = new CalleefptrStmt(stream);
//		}
//		else {
//			cout << "wrong stmt type!!!" << endl;
//			exit(EXIT_FAILURE);
//		}
//	}
    CFGNode(std::string& line) {
	    std::stringstream stream(line);
		std::string stmt_id;
		stream >> stmt_id;
        this->id = atoi(stmt_id.c_str());
        std::string arg;
        while(stream >> arg) {
            int address = 0;
            //address = hexToDec(arg);
            sscanf(arg.c_str(), "%x", &address);
            //cout << address << endl;
            content.push_back(address);
        }
	}

    ~CFGNode(){
    	if(stmt){
			delete stmt;
    	}
    }

    int getHexValue(char a) {

        int tmp = 0;
        if (a >= '0'&&a <= '9')
            tmp = a - '0';
        else {
            switch (a)
            {
                case 'a':
                case 'A':
                    tmp = 10;
                    break;
                case 'b':
                case 'B':
                    tmp = 11;
                    break;
                case 'c':
                case 'C':
                    tmp = 12;
                    break;
                case 'd':
                case 'D':
                    tmp = 13;
                    break;
                case 'e':
                case 'E':
                    tmp = 14;
                    break;
                case 'f':
                case 'F':
                    tmp = 15;
                    break;
                default:
                    tmp = 0;
                    break;
            }

        }

        return tmp;
    }
    int  hexToDec(std::string addr) {
        string addrH = addr.substr(2); // 以0x40063b为例，截取后面具体的16进制地址
        int sum = 0;
        int len = addrH.length();
        int pow = 1;
        for (int i = 0; i <len; i++) {
            sum += getHexValue(addrH[len - i - 1])*pow;
            pow = pow * 16;
        }


        return sum;  //行内偏移  sum = sum / 64;
        //组号 sum = sum % 128;
    }

    size_t get_size_bytes() const {
//    	return sizeof(PEGraph_Pointer) + sizeof(TYPE) + stmt->get_size_bytes();
        return sizeof(PEGraph_Pointer) + sizeof(std::vector<IR>::size_type) + sizeof(IR) * content.size();
    }

    void write_to_buf(Buffer& buf){
    	size_t s = get_size_bytes();
    	memcpy(buf.getData() + buf.getSize(), (char*)& s, sizeof(size_t));
    	buf.add_size_by(sizeof(size_t));

    	memcpy(buf.getData() + buf.getSize(), (char*)& id, sizeof(PEGraph_Pointer));
    	buf.add_size_by(sizeof(PEGraph_Pointer));

    	std::vector<IR>::size_type size = content.size();
        memcpy(buf.getData() + buf.getSize(), (char*)& size, sizeof(std::vector<IR>::size_type));
        buf.add_size_by(sizeof(std::vector<IR>::size_type));


        for (int i = 0; i < content.size(); ++i) {
            IR ir = content[i];
            memcpy(buf.getData() + buf.getSize(), (char*)& ir, sizeof(IR));
            buf.add_size_by(sizeof(IR));
        }


//    	TYPE t = stmt->getType();
//    	memcpy(buf.getData() + buf.getSize(), (char*)& t, sizeof(TYPE));
//    	buf.add_size_by(sizeof(TYPE));
//    	stmt->write_to_buf(buf);
    }

    void read_from_buf(char* buf, size_t bufsize){
    	size_t offset = 0;
    	id = *((PEGraph_Pointer*)(buf));
    	offset += sizeof(PEGraph_Pointer);
    	std::vector<IR>::size_type size = *((std::vector<IR>::size_type*)(buf + offset));
        offset += sizeof(std::vector<IR>::size_type);
        for (int i = 0; i < size; i++) {
            IR ir = *((IR*)(buf + offset));
            content.push_back(ir);
            offset += sizeof(IR);
        }
//    	TYPE type = *((TYPE*)(buf + offset));
//    	offset += sizeof(TYPE);
//
//    	switch(type){
//    	case TYPE::Assign: stmt = new AssignStmt(); break;
//    	case TYPE::Load: stmt = new LoadStmt(); break;
//    	case TYPE::Store: stmt = new StoreStmt(); break;
//    	case TYPE::Alloca: stmt = new AllocStmt(); break;
//    	case TYPE::Phi: stmt = new PhiStmt(); break;
//    	case TYPE::Call: stmt = new CallStmt(); break;
//    	case TYPE::Return: stmt = new ReturnStmt(); break;
//    	case TYPE::Ret: stmt = new RetStmt(); break;
//    	case TYPE::Skip: stmt = new SkipStmt(); break;
//    	case TYPE::Callfptr: stmt = new CallfptrStmt(); break;
//    	case TYPE::Calleefptr: stmt = new CalleefptrStmt(); break;
//    	default:
//    		cout << "wrong stmt type!!!" << endl;
//    		exit(EXIT_FAILURE);
//    	}
//
//    	stmt->read_from_buf(buf, offset, bufsize);
    }

    inline Stmt* getStmt() const {
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

    inline void setID(int id) {
        this->id = id;
    }

    inline void addIR(IR inst) {
        content.push_back(inst);
    }

    inline std::vector<int> getAllContent() {
        return content;
    }






private:
    PEGraph_Pointer id;

    std::vector<IR> content;

    Stmt* stmt = nullptr;



};



#endif /* COMP_CFG_NODE_H_ */
