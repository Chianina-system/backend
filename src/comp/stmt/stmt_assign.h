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




private:
	vertexid_t src;
	vertexid_t dst;

    void toString_sub(std::ostringstream& strm) const {
    	strm << "assign" << ", " << getDst() << " <- " << getSrc();
    }


};



#endif /* STMT_ASSIGN_H_ */
