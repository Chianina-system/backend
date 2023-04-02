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
		// if(cfgNode.getStmt()){
		// 	strm << cfgNode.getStmt()->toString();
		// }
		return strm;
	}

public:
    CFGNode(){
    	id = -1;
    }

    CFGNode(int _id) {
    	id = _id;
    }

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
            content.push_back(address);
        }
	}

    ~CFGNode(){
//    	if(stmt){
//			delete stmt;
//    	}
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
    }

    inline Stmt* getStmt() const {
        //return stmt;
        return nullptr;
    }

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
};



#endif /* COMP_CFG_NODE_H_ */
