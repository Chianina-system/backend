/*
 * stmt_phi.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef COMP_STMT_PHI_H_
#define COMP_STMT_PHI_H_

#include "stmt.h"

class PhiStmt: public Stmt {

public:
	PhiStmt(){
		this->t = TYPE::Phi;
		this->length = 0;
		this->src = NULL;
		this->dst = -1;
	}

	~PhiStmt(){
    	if(this->src){
    		delete[] src;
    	}
	}

	PhiStmt(std::stringstream& stream){
		this->t = TYPE::Phi;

		std::string dst, src;

		stream >> dst;
		this->dst = atoi(dst.c_str());

		std::unordered_set<std::string> set_string;
		while(getline(stream, src, '\t')){
			set_string.insert(src);
		}

		this->length = set_string.size();
		this->src = new vertexid_t[this->length];
		int i = 0;
		for(auto& it: set_string){
			this->src[i] = atoi(it.c_str());
			i++;
		}
	}

	int getLength() const {
		return length;
	}

	vertexid_t* getSrcs() const {
		return src;
	}

	vertexid_t getDst() const{
		return dst;
	}


    size_t get_size_bytes() const {
    	return sizeof(vertexid_t) + sizeof(vertexid_t) * length;
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), src, sizeof(vertexid_t) * length);
    	buf.add_size_by(sizeof(vertexid_t) * length);
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	dst = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	length = (bufsize - offset) / sizeof(vertexid_t);
    	this->src = new vertexid_t[length];
    	memcpy(src, buf + offset, sizeof(vertexid_t) * length);
    }

private:

    int length;
	vertexid_t* src;
	vertexid_t dst;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "phi" << ", " << getDst() << " <- [";
    	for(int i = 0; i < length; i++){
    		strm << src[i] << ", ";
    	}
    	strm << "]";
    }


};

#endif /* COMP_STMT_PHI_H_ */
