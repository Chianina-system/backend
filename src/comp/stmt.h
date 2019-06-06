/*
 * Stmt.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_STMT_H_
#define COMP_STMT_H_
#include "../common/CommonLibs.hpp"

enum class TYPE {
	Copy,
	Load,
	Store,
	Address
};

class Stmt{

public:


	TYPE getType() const{
		return t;
	}

	vertexid_t getSrc() const{
		return src;	
	}

	vertexid_t getDst() const{
		return dst;	
	}

private:
	TYPE t;
	vertexid_t src;
	vertexid_t dst;

};




#endif /* COMP_STMT_H_ */
