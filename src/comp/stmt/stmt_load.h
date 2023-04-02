/*
 * stmt_load.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef STMT_LOAD_H_
#define STMT_LOAD_H_

#include "stmt.h"

class LoadStmt: public Stmt {

public:
	LoadStmt(){
		this->t = TYPE::Load;
		this->src = -1;
		this->dst = -1;
		this->auxiliary = -1;
	}

	~LoadStmt(){}

	LoadStmt(std::stringstream& stream){
		this->t = TYPE::Load;

		std::string dst, src, aux;
		stream >> dst >> src >> aux;
		this->src = atoi(src.c_str());
		this->dst = atoi(dst.c_str());
		this->auxiliary = atoi(aux.c_str());
	}


	vertexid_t getSrc() const{
		return src;
	}

	vertexid_t getDst() const{
		return dst;
	}

    vertexid_t getAux() const{
        return auxiliary;
    }

    size_t get_size_bytes() const {
    	return sizeof(vertexid_t) * 3;
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& src, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), (char*)& auxiliary, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	src = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	dst = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	auxiliary = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	assert(offset == bufsize);
    }



private:

	vertexid_t src;
	vertexid_t dst;
	vertexid_t auxiliary;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "load" << ", " << getDst() << " <- " << getSrc() << ", " << getAux();
    }

};



#endif /* STMT_LOAD_H_ */
