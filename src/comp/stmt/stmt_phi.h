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



private:

    int length;
	vertexid_t* src = nullptr;
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
