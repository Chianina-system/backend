/*
 * stmt_calleefptr.h
 *
 *  Created on: April 20, 2020
 *      Author: dell
 */

#ifndef STMT_CALLEEFPTR_H_
#define STMT_CALLEEFPTR_H_

#include "stmt.h"

class CalleefptrStmt: public Stmt {

public:
	CalleefptrStmt(){
		this->t = TYPE::Calleefptr;
		this->dst = -1;
	}

	~CalleefptrStmt(){}

	CalleefptrStmt(std::stringstream& stream){
		this->t = TYPE::Calleefptr;

		std::string dst;
		stream >> dst;
		this->dst = atoi(dst.c_str());
	}


	vertexid_t getDst() const{
		return dst;
	}

    size_t get_size_bytes() const {
    	return sizeof(vertexid_t);
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	dst = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	assert(offset == bufsize);
    }



private:

	vertexid_t dst;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "calleefptr" << ", " << getDst();
    }

};



#endif /* STMT_CALLEEFPTR_H_ */
