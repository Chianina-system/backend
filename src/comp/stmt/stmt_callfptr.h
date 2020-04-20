/*
 * stmt_callfptr.h
 *
 *  Created on: April 20, 2020
 *      Author: dell
 */

#ifndef STMT_CALLFPTR_H_
#define STMT_CALLFPTR_H_

#include "stmt.h"

class CallfptrStmt: public Stmt {

public:
	CallfptrStmt(){
		this->t = TYPE::Callfptr;
		this->dst = -1;
		this->auxiliary = -1;
	}

	~CallfptrStmt(){}

	CallfptrStmt(std::stringstream& stream){
		this->t = TYPE::Callfptr;

		std::string dst, aux;
		stream >> dst >> aux;
		this->dst = atoi(dst.c_str());
		this->auxiliary = atoi(aux.c_str());
	}


	vertexid_t getDst() const{
		return dst;
	}

    vertexid_t getAux() const{
        return auxiliary;
    }

    size_t get_size_bytes() const {
    	return sizeof(vertexid_t) * 2;
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), (char*)& auxiliary, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	dst = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	auxiliary = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	assert(offset == bufsize);
    }



private:

	vertexid_t dst;
	vertexid_t auxiliary;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "callfptr" << ", " << getDst() << ", " << getAux();
    }

};



#endif /* STMT_CALLFPTR_H_ */
