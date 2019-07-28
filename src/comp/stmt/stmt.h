/*
 * Stmt.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef STMT_H_
#define STMT_H_
#include "../../common/CommonLibs.hpp"

enum class TYPE {
	Assign,
	Load,
	Store,
	Alloca,
	Phi,
	Call,
	Return,
	Ret,
	Skip
};

class Stmt{
//	friend std::ostream & operator<<(std::ostream & strm, const Stmt& stmt) {
//		return strm;
//	}


public:
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

//    std::string getTypeString(TYPE t){
//    	switch(t){
//    	case TYPE::Assign: return "Assign";
//    	case TYPE::Load: return "Load";
//    	case TYPE::Store: return "Store";
//    	case TYPE::Alloca: return "Alloca";
//    	case TYPE::Phi: return "Phi";
//    	case TYPE::Call: return "Call";
//    	case TYPE::Return: return "Return";
//    	case TYPE::Ret: return "Ret";
//    	case TYPE::Skip: return "Skip";
//    	default:
//    		return nullptr;
//    	}
//    }


private:
    virtual void toString_sub(std::ostringstream& strm) const = 0;



protected:
	TYPE t;

};




#endif /* STMT_H_ */
