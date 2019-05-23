/*
 * Stmt.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_STMT_H_
#define COMP_STMT_H_


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



private:
	TYPE t;





};




#endif /* COMP_STMT_H_ */
