/*
 * Stmt.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef STMT_H_
#define STMT_H_
#include "../../common/CommonLibs.hpp"
#include "../graphstore/buffer.h"

enum class TYPE {
	Assign = 1,
	Load,
	Store,
	Alloca,
	Phi,
	Call,
	Return,
	Ret,
	Skip,
	Callfptr,
	Calleefptr
};

class Stmt{
//	friend std::ostream & operator<<(std::ostream & strm, const Stmt& stmt) {
//		return strm;
//	}


public:
	Stmt(){}

	virtual ~Stmt(){}


	TYPE getType() const{
		return t;
	}

    std::string toString(){
    	std::ostringstream out;
    	out << "(";
    	toString_sub(out);
    	out << ")";
    	return out.str();
    }

//    char getTypeChar(TYPE t){
//    	switch(t){
//    	case TYPE::Assign: return 1;
//    	case TYPE::Load: return 2;
//    	case TYPE::Store: return 3;
//    	case TYPE::Alloca: return 4;
//    	case TYPE::Phi: return 5;
//    	case TYPE::Call: return 6;
//    	case TYPE::Return: return 7;
//    	case TYPE::Ret: return 8;
//    	case TYPE::Skip: return 9;
//    	default:
//    		return 0;
//    	}
//    }
//
//    char getTypeChar(){
//    	return getTypeChar(t);
//    }


    virtual size_t get_size_bytes() const = 0;

    virtual void write_to_buf(Buffer& buf) = 0;

    virtual void read_from_buf(char* buf, size_t offset, size_t bufsize) = 0;

private:
    virtual void toString_sub(std::ostringstream& strm) const = 0;



protected:
	TYPE t;

};




#endif /* STMT_H_ */
