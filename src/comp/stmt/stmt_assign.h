/*
 * stmt_assign.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef STMT_ASSIGN_H_
#define STMT_ASSIGN_H_

#include "stmt.h"

class AssignStmt: public Stmt {

public:
	AssignStmt(){
		this->t = TYPE::Assign;
		this->src = -1;
		this->dst = -1;
	}

	~AssignStmt(){}

	AssignStmt(std::stringstream& stream){
		this->t = TYPE::Assign;

		std::string dst, src;
		stream >> dst >> src;
		this->src = atoi(src.c_str());
		this->dst = atoi(dst.c_str());
	}

	vertexid_t getSrc() const{
		return src;
	}

	vertexid_t getDst() const{
		return dst;
	}

    size_t get_size_bytes() const {
    	return sizeof(vertexid_t) * 2;
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& src, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	src = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	dst = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	assert(offset == bufsize);
    }



private:
	vertexid_t src;
	vertexid_t dst;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "assign" << ", " << getDst() << " <- " << getSrc();
    }


};



#endif /* STMT_ASSIGN_H_ */
