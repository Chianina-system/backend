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



private:

	vertexid_t src;
	vertexid_t dst;
	vertexid_t auxiliary;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "load" << ", " << getDst() << " <- " << getSrc() << ", " << getAux();
    }

};



#endif /* STMT_LOAD_H_ */
